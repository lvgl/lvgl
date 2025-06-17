#!/usr/bin/ruby

# ==========================================
#   Unity Project - A Test Framework for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

class UnityTestRunnerGenerator
  def initialize(options = nil)
    @options = UnityTestRunnerGenerator.default_options
    case options
    when NilClass
      @options
    when String
      @options.merge!(UnityTestRunnerGenerator.grab_config(options))
    when Hash
      # Check if some of these have been specified
      @options[:has_setup] = !options[:setup_name].nil?
      @options[:has_teardown] = !options[:teardown_name].nil?
      @options[:has_suite_setup] = !options[:suite_setup].nil?
      @options[:has_suite_teardown] = !options[:suite_teardown].nil?
      @options.merge!(options)
    else
      raise 'If you specify arguments, it should be a filename or a hash of options'
    end
    require_relative 'type_sanitizer'
  end

  def self.default_options
    {
      includes: [],
      defines: [],
      plugins: [],
      framework: :unity,
      test_prefix: 'test|spec|should',
      mock_prefix: 'Mock',
      mock_suffix: '',
      setup_name: 'setUp',
      teardown_name: 'tearDown',
      test_reset_name: 'resetTest',
      test_verify_name: 'verifyTest',
      main_name: 'main', # set to :auto to automatically generate each time
      main_export_decl: '',
      cmdline_args: false,
      omit_begin_end: false,
      use_param_tests: false,
      use_system_files: true,
      include_extensions: '(?:hpp|hh|H|h)',
      source_extensions: '(?:cpp|cc|ino|C|c)'
    }
  end

  def self.grab_config(config_file)
    options = default_options
    unless config_file.nil? || config_file.empty?
      require_relative 'yaml_helper'
      yaml_guts = YamlHelper.load_file(config_file)
      options.merge!(yaml_guts[:unity] || yaml_guts[:cmock])
      raise "No :unity or :cmock section found in #{config_file}" unless options
    end
    options
  end

  def run(input_file, output_file, options = nil)
    @options.merge!(options) unless options.nil?

    # pull required data from source file
    source = File.read(input_file)
    source = source.force_encoding('ISO-8859-1').encode('utf-8', replace: nil)
    tests = find_tests(source)
    headers = find_includes(source)
    testfile_includes = @options[:use_system_files] ? (headers[:local] + headers[:system]) : (headers[:local])
    used_mocks = find_mocks(testfile_includes)
    testfile_includes = (testfile_includes - used_mocks)
    testfile_includes.delete_if { |inc| inc =~ /(unity|cmock)/ }
    find_setup_and_teardown(source)

    # build runner file
    generate(input_file, output_file, tests, used_mocks, testfile_includes)

    # determine which files were used to return them
    all_files_used = [input_file, output_file]
    all_files_used += testfile_includes.map { |filename| "#{filename}.c" } unless testfile_includes.empty?
    all_files_used += @options[:includes] unless @options[:includes].empty?
    all_files_used += headers[:linkonly] unless headers[:linkonly].empty?
    all_files_used.uniq
  end

  def generate(input_file, output_file, tests, used_mocks, testfile_includes)
    File.open(output_file, 'w') do |output|
      create_header(output, used_mocks, testfile_includes)
      create_externs(output, tests, used_mocks)
      create_mock_management(output, used_mocks)
      create_setup(output)
      create_teardown(output)
      create_suite_setup(output)
      create_suite_teardown(output)
      create_reset(output)
      create_run_test(output) unless tests.empty?
      create_args_wrappers(output, tests)
      create_main(output, input_file, tests, used_mocks)
    end

    return unless @options[:header_file] && !@options[:header_file].empty?

    File.open(@options[:header_file], 'w') do |output|
      create_h_file(output, @options[:header_file], tests, testfile_includes, used_mocks)
    end
  end

  def find_tests(source)
    tests_and_line_numbers = []

    # contains characters which will be substituted from within strings, doing
    # this prevents these characters from interfering with scrubbers
    # @ is not a valid C character, so there should be no clashes with files genuinely containing these markers
    substring_subs = { '{' => '@co@', '}' => '@cc@', ';' => '@ss@', '/' => '@fs@' }
    substring_re = Regexp.union(substring_subs.keys)
    substring_unsubs = substring_subs.invert                   # the inverse map will be used to fix the strings afterwards
    substring_unsubs['@quote@'] = '\\"'
    substring_unsubs['@apos@'] = '\\\''
    substring_unre = Regexp.union(substring_unsubs.keys)
    source_scrubbed = source.clone
    source_scrubbed = source_scrubbed.gsub(/\\"/, '@quote@')   # hide escaped quotes to allow capture of the full string/char
    source_scrubbed = source_scrubbed.gsub(/\\'/, '@apos@')    # hide escaped apostrophes to allow capture of the full string/char
    source_scrubbed = source_scrubbed.gsub(/("[^"\n]*")|('[^'\n]*')/) { |s| s.gsub(substring_re, substring_subs) } # temporarily hide problematic characters within strings
    source_scrubbed = source_scrubbed.gsub(/\/\/(?:.+\/\*|\*(?:$|[^\/])).*$/, '')  # remove line comments that comment out the start of blocks
    source_scrubbed = source_scrubbed.gsub(/\/\*.*?\*\//m, '')                     # remove block comments
    source_scrubbed = source_scrubbed.gsub(/\/\/.*$/, '')                          # remove line comments (all that remain)
    lines = source_scrubbed.split(/(^\s*\#.*$) | (;|\{|\}) /x)                     # Treat preprocessor directives as a logical line. Match ;, {, and } as end of lines
                           .map { |line| line.gsub(substring_unre, substring_unsubs) } # unhide the problematic characters previously removed

    lines.each_with_index do |line, _index|
      # find tests
      next unless line =~ /^((?:\s*(?:TEST_(?:CASE|RANGE|MATRIX))\s*\(.*?\)\s*)*)\s*void\s+((?:#{@options[:test_prefix]}).*)\s*\(\s*(.*)\s*\)/m
      next unless line =~ /^((?:\s*(?:TEST_(?:CASE|RANGE|MATRIX))\s*\(.*?\)\s*)*)\s*void\s+((?:#{@options[:test_prefix]})\w*)\s*\(\s*(.*)\s*\)/m

      arguments = Regexp.last_match(1)
      name = Regexp.last_match(2)
      call = Regexp.last_match(3)
      params = Regexp.last_match(4)
      args = nil

      if @options[:use_param_tests] && !arguments.empty?
        args = []
        type_and_args = arguments.split(/TEST_(CASE|RANGE|MATRIX)/)
        (1...type_and_args.length).step(2).each do |i|
          case type_and_args[i]
          when 'CASE'
            args << type_and_args[i + 1].sub(/^\s*\(\s*(.*?)\s*\)\s*$/m, '\1')

          when 'RANGE'
            args += type_and_args[i + 1].scan(/(\[|<)\s*(-?\d+.?\d*)\s*,\s*(-?\d+.?\d*)\s*,\s*(-?\d+.?\d*)\s*(\]|>)/m).map do |arg_values_str|
              exclude_end = arg_values_str[0] == '<' && arg_values_str[-1] == '>'
              arg_values_str[1...-1].map do |arg_value_str|
                arg_value_str.include?('.') ? arg_value_str.to_f : arg_value_str.to_i
              end.push(exclude_end)
            end.map do |arg_values|
              Range.new(arg_values[0], arg_values[1], arg_values[3]).step(arg_values[2]).to_a
            end.reduce(nil) do |result, arg_range_expanded|
              result.nil? ? arg_range_expanded.map { |a| [a] } : result.product(arg_range_expanded)
            end.map do |arg_combinations|
              arg_combinations.flatten.join(', ')
            end

          when 'MATRIX'
            single_arg_regex_string = /(?:(?:"(?:\\"|[^\\])*?")+|(?:'\\?.')+|(?:[^\s\]\["',]|\[[\d\S_-]+\])+)/.source
            args_regex = /\[((?:\s*#{single_arg_regex_string}\s*,?)*(?:\s*#{single_arg_regex_string})?\s*)\]/m
            arg_elements_regex = /\s*(#{single_arg_regex_string})\s*,\s*/m

            args += type_and_args[i + 1].scan(args_regex).flatten.map do |arg_values_str|
              ("#{arg_values_str},").scan(arg_elements_regex)
            end.reduce do |result, arg_range_expanded|
              result.product(arg_range_expanded)
            end.map do |arg_combinations|
              arg_combinations.flatten.join(', ')
            end
          end
        end
      end

      tests_and_line_numbers << { test: name, args: args, call: call, params: params, line_number: 0 }
    end

    tests_and_line_numbers.uniq! { |v| v[:test] }

    # determine line numbers and create tests to run
    source_lines = source.split("\n")
    source_index = 0
    tests_and_line_numbers.size.times do |i|
      source_lines[source_index..].each_with_index do |line, index|
        next unless line =~ /\s+#{tests_and_line_numbers[i][:test]}(?:\s|\()/

        source_index += index
        tests_and_line_numbers[i][:line_number] = source_index + 1
        break
      end
    end

    tests_and_line_numbers
  end

  def find_includes(source)
    # remove comments (block and line, in three steps to ensure correct precedence)
    source.gsub!(/\/\/(?:.+\/\*|\*(?:$|[^\/])).*$/, '')  # remove line comments that comment out the start of blocks
    source.gsub!(/\/\*.*?\*\//m, '')                     # remove block comments
    source.gsub!(/\/\/.*$/, '')                          # remove line comments (all that remain)

    # parse out includes
    {
      local: source.scan(/^\s*#include\s+"\s*(.+\.#{@options[:include_extensions]})\s*"/).flatten,
      system: source.scan(/^\s*#include\s+<\s*(.+)\s*>/).flatten.map { |inc| "<#{inc}>" },
      linkonly: source.scan(/^TEST_SOURCE_FILE\(\s*"\s*(.+\.#{@options[:source_extensions]})\s*"/).flatten
    }
  end

  def find_mocks(includes)
    mock_headers = []
    includes.each do |include_path|
      include_file = File.basename(include_path)
      mock_headers << include_path if include_file =~ /^#{@options[:mock_prefix]}.*#{@options[:mock_suffix]}\.h$/i
    end
    mock_headers
  end

  def find_setup_and_teardown(source)
    @options[:has_setup] = source =~ /void\s+#{@options[:setup_name]}\s*\(/
    @options[:has_teardown] = source =~ /void\s+#{@options[:teardown_name]}\s*\(/
    @options[:has_suite_setup] ||= (source =~ /void\s+suiteSetUp\s*\(/)
    @options[:has_suite_teardown] ||= (source =~ /int\s+suiteTearDown\s*\(int\s+([a-zA-Z0-9_])+\s*\)/)
  end

  def create_header(output, mocks, testfile_includes = [])
    output.puts('/* AUTOGENERATED FILE. DO NOT EDIT. */')
    output.puts("\n/*=======Automagically Detected Files To Include=====*/")
    output.puts('extern "C" {') if @options[:externcincludes]
    output.puts("#include \"#{@options[:framework]}.h\"")
    output.puts('#include "cmock.h"') unless mocks.empty?
    output.puts('}') if @options[:externcincludes]
    if @options[:defines] && !@options[:defines].empty?
      output.puts("/* injected defines for unity settings, etc */")
      @options[:defines].each do |d| 
        def_only = d.match(/(\w+).*/)[1]
        output.puts("#ifndef #{def_only}\n#define #{d}\n#endif /* #{def_only} */")
      end
    end
    if @options[:header_file] && !@options[:header_file].empty?
      output.puts("#include \"#{File.basename(@options[:header_file])}\"")
    else
      @options[:includes].flatten.uniq.compact.each do |inc|
        output.puts("#include #{inc.include?('<') ? inc : "\"#{inc}\""}")
      end
      testfile_includes.each do |inc|
        output.puts("#include #{inc.include?('<') ? inc : "\"#{inc}\""}")
      end
    end
    output.puts('extern "C" {') if @options[:externcincludes]
    mocks.each do |mock|
      output.puts("#include \"#{mock}\"")
    end
    output.puts('}') if @options[:externcincludes]
    output.puts('#include "CException.h"') if @options[:plugins].include?(:cexception)

    return unless @options[:enforce_strict_ordering]

    output.puts('')
    output.puts('int GlobalExpectCount;')
    output.puts('int GlobalVerifyOrder;')
    output.puts('char* GlobalOrderError;')
  end

  def create_externs(output, tests, _mocks)
    output.puts("\n/*=======External Functions This Runner Calls=====*/")
    output.puts("extern void #{@options[:setup_name]}(void);")
    output.puts("extern void #{@options[:teardown_name]}(void);")
    output.puts("\n#ifdef __cplusplus\nextern \"C\"\n{\n#endif") if @options[:externc]
    tests.each do |test|
      output.puts("extern void #{test[:test]}(#{test[:call] || 'void'});")
    end
    output.puts("#ifdef __cplusplus\n}\n#endif") if @options[:externc]
    output.puts('')
  end

  def create_mock_management(output, mock_headers)
    output.puts("\n/*=======Mock Management=====*/")
    output.puts('static void CMock_Init(void)')
    output.puts('{')

    if @options[:enforce_strict_ordering]
      output.puts('  GlobalExpectCount = 0;')
      output.puts('  GlobalVerifyOrder = 0;')
      output.puts('  GlobalOrderError = NULL;')
    end

    mocks = mock_headers.map { |mock| File.basename(mock, '.*') }
    mocks.each do |mock|
      mock_clean = TypeSanitizer.sanitize_c_identifier(mock)
      output.puts("  #{mock_clean}_Init();")
    end
    output.puts("}\n")

    output.puts('static void CMock_Verify(void)')
    output.puts('{')
    mocks.each do |mock|
      mock_clean = TypeSanitizer.sanitize_c_identifier(mock)
      output.puts("  #{mock_clean}_Verify();")
    end
    output.puts("}\n")

    output.puts('static void CMock_Destroy(void)')
    output.puts('{')
    mocks.each do |mock|
      mock_clean = TypeSanitizer.sanitize_c_identifier(mock)
      output.puts("  #{mock_clean}_Destroy();")
    end
    output.puts("}\n")
  end

  def create_setup(output)
    return if @options[:has_setup]

    output.puts("\n/*=======Setup (stub)=====*/")
    output.puts("void #{@options[:setup_name]}(void) {}")
  end

  def create_teardown(output)
    return if @options[:has_teardown]

    output.puts("\n/*=======Teardown (stub)=====*/")
    output.puts("void #{@options[:teardown_name]}(void) {}")
  end

  def create_suite_setup(output)
    return if @options[:suite_setup].nil?

    output.puts("\n/*=======Suite Setup=====*/")
    output.puts('void suiteSetUp(void)')
    output.puts('{')
    output.puts(@options[:suite_setup])
    output.puts('}')
  end

  def create_suite_teardown(output)
    return if @options[:suite_teardown].nil?

    output.puts("\n/*=======Suite Teardown=====*/")
    output.puts('int suiteTearDown(int num_failures)')
    output.puts('{')
    output.puts(@options[:suite_teardown])
    output.puts('}')
  end

  def create_reset(output)
    output.puts("\n/*=======Test Reset Options=====*/")
    output.puts("void #{@options[:test_reset_name]}(void);")
    output.puts("void #{@options[:test_reset_name]}(void)")
    output.puts('{')
    output.puts("  #{@options[:teardown_name]}();")
    output.puts('  CMock_Verify();')
    output.puts('  CMock_Destroy();')
    output.puts('  CMock_Init();')
    output.puts("  #{@options[:setup_name]}();")
    output.puts('}')
    output.puts("void #{@options[:test_verify_name]}(void);")
    output.puts("void #{@options[:test_verify_name]}(void)")
    output.puts('{')
    output.puts('  CMock_Verify();')
    output.puts('}')
  end

  def create_run_test(output)
    require 'erb'
    file = File.read(File.join(__dir__, 'run_test.erb'))
    template = ERB.new(file, trim_mode: '<>')
    output.puts("\n#{template.result(binding)}")
  end

  def create_args_wrappers(output, tests)
    return unless @options[:use_param_tests]

    output.puts("\n/*=======Parameterized Test Wrappers=====*/")
    tests.each do |test|
      next if test[:args].nil? || test[:args].empty?

      test[:args].each.with_index(1) do |args, idx|
        output.puts("static void runner_args#{idx}_#{test[:test]}(void)")
        output.puts('{')
        output.puts("    #{test[:test]}(#{args});")
        output.puts("}\n")
      end
    end
  end

  def create_main(output, filename, tests, used_mocks)
    output.puts("\n/*=======MAIN=====*/")
    main_name = @options[:main_name].to_sym == :auto ? "main_#{filename.gsub('.c', '')}" : (@options[:main_name]).to_s
    if @options[:cmdline_args]
      if main_name != 'main'
        output.puts("#{@options[:main_export_decl]} int #{main_name}(int argc, char** argv);")
      end
      output.puts("#{@options[:main_export_decl]} int #{main_name}(int argc, char** argv)")
      output.puts('{')
      output.puts('  int parse_status = UnityParseOptions(argc, argv);')
      output.puts('  if (parse_status != 0)')
      output.puts('  {')
      output.puts('    if (parse_status < 0)')
      output.puts('    {')
      output.puts("      UnityPrint(\"#{filename.gsub('.c', '').gsub(/\\/, '\\\\\\')}.\");")
      output.puts('      UNITY_PRINT_EOL();')
      tests.each do |test|
        if (!@options[:use_param_tests]) || test[:args].nil? || test[:args].empty?
          output.puts("      UnityPrint(\"  #{test[:test]}\");")
          output.puts('      UNITY_PRINT_EOL();')
        else
          test[:args].each do |args|
            output.puts("      UnityPrint(\"  #{test[:test]}(#{args})\");")
            output.puts('      UNITY_PRINT_EOL();')
          end
        end
      end
      output.puts('      return 0;')
      output.puts('    }')
      output.puts('    return parse_status;')
      output.puts('  }')
    else
      main_return = @options[:omit_begin_end] ? 'void' : 'int'
      if main_name != 'main'
        output.puts("#{@options[:main_export_decl]} #{main_return} #{main_name}(void);")
      end
      output.puts("#{main_return} #{main_name}(void)")
      output.puts('{')
    end
    output.puts('  suiteSetUp();') if @options[:has_suite_setup]
    if @options[:omit_begin_end]
      output.puts("  UnitySetTestFile(\"#{filename.gsub(/\\/, '\\\\\\')}\");")
    else
      output.puts("  UnityBegin(\"#{filename.gsub(/\\/, '\\\\\\')}\");")
    end
    tests.each do |test|
      if (!@options[:use_param_tests]) || test[:args].nil? || test[:args].empty?
        output.puts("  run_test(#{test[:test]}, \"#{test[:test]}\", #{test[:line_number]});")
      else
        test[:args].each.with_index(1) do |args, idx|
          wrapper = "runner_args#{idx}_#{test[:test]}"
          testname = "#{test[:test]}(#{args})".dump
          output.puts("  run_test(#{wrapper}, #{testname}, #{test[:line_number]});")
        end
      end
    end
    output.puts
    output.puts('  CMock_Guts_MemFreeFinal();') unless used_mocks.empty?
    if @options[:has_suite_teardown]
      if @options[:omit_begin_end]
        output.puts('  (void) suite_teardown(0);')
      else
        output.puts('  return suiteTearDown(UnityEnd());')
      end
    else
      output.puts('  return UnityEnd();') unless @options[:omit_begin_end]
    end
    output.puts('}')
  end

  def create_h_file(output, filename, tests, testfile_includes, used_mocks)
    filename = File.basename(filename).gsub(/[-\/\\.,\s]/, '_').upcase
    output.puts('/* AUTOGENERATED FILE. DO NOT EDIT. */')
    output.puts("#ifndef _#{filename}")
    output.puts("#define _#{filename}\n\n")
    output.puts("#include \"#{@options[:framework]}.h\"")
    output.puts('#include "cmock.h"') unless used_mocks.empty?
    @options[:includes].flatten.uniq.compact.each do |inc|
      output.puts("#include #{inc.include?('<') ? inc : "\"#{inc}\""}")
    end
    testfile_includes.each do |inc|
      output.puts("#include #{inc.include?('<') ? inc : "\"#{inc}\""}")
    end
    output.puts "\n"
    tests.each do |test|
      if test[:params].nil? || test[:params].empty?
        output.puts("void #{test[:test]}(void);")
      else
        output.puts("void #{test[:test]}(#{test[:params]});")
      end
    end
    output.puts("#endif\n\n")
  end
end

if $0 == __FILE__
  options = { includes: [] }

  # parse out all the options first (these will all be removed as we go)
  ARGV.reject! do |arg|
    case arg
    when '-cexception'
      options[:plugins] = [:cexception]
      true
    when '-externcincludes'
      options[:externcincludes] = true
      true
    when /\.*\.ya?ml$/
      options = UnityTestRunnerGenerator.grab_config(arg)
      true
    when /--(\w+)="?(.*)"?/
      options[Regexp.last_match(1).to_sym] = Regexp.last_match(2)
      true
    when /\.*\.(?:hpp|hh|H|h)$/
      options[:includes] << arg
      true
    else false
    end
  end

  # make sure there is at least one parameter left (the input file)
  unless ARGV[0]
    puts ["\nusage: ruby #{__FILE__} (files) (options) input_test_file (output)",
          "\n  input_test_file         - this is the C file you want to create a runner for",
          '  output                  - this is the name of the runner file to generate',
          '                            defaults to (input_test_file)_Runner',
          '  files:',
          '    *.yml / *.yaml        - loads configuration from here in :unity or :cmock',
          '    *.h                   - header files are added as #includes in runner',
          '  options:',
          '    -cexception           - include cexception support',
          '    -externc              - add extern "C" for cpp support',
          '    --setup_name=""       - redefine setUp func name to something else',
          '    --teardown_name=""    - redefine tearDown func name to something else',
          '    --main_name=""        - redefine main func name to something else',
          '    --test_prefix=""      - redefine test prefix from default test|spec|should',
          '    --test_reset_name=""  - redefine resetTest func name to something else',
          '    --test_verify_name="" - redefine verifyTest func name to something else',
          '    --suite_setup=""      - code to execute for setup of entire suite',
          '    --suite_teardown=""   - code to execute for teardown of entire suite',
          '    --use_param_tests=1   - enable parameterized tests (disabled by default)',
          '    --omit_begin_end=1    - omit calls to UnityBegin and UnityEnd (disabled by default)',
          '    --header_file=""      - path/name of test header file to generate too'].join("\n")
    exit 1
  end

  # create the default test runner name if not specified
  ARGV[1] = ARGV[0].gsub('.c', '_Runner.c') unless ARGV[1]

  UnityTestRunnerGenerator.new(options).run(ARGV[0], ARGV[1])
end
