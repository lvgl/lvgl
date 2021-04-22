```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/porting/log.md
```
# Logging

LVGL has built-in *log* module to inform the user about what is happening in the library.

## Log level
To enable logging, set `LV_USE_LOG  1` in *lv_conf.h* and set `LV_LOG_LEVEL` to one of the following values:
- **LV_LOG_LEVEL_TRACE** A lot of logs to give detailed information
- **LV_LOG_LEVEL_INFO**  Log important events
- **LV_LOG_LEVEL_WARN**  Log if something unwanted happened but didn't cause a problem
- **LV_LOG_LEVEL_ERROR** Only critical issue, when the system may fail
- **LV_LOG_LEVEL_NONE**  Do not log anything

The events which have a higher level than the set log level will be logged too. E.g. if you `LV_LOG_LEVEL_WARN`, *errors* will be also logged.

## Logging with printf
If your system supports `printf`, you just need to enable `LV_LOG_PRINTF` in *lv_conf.h* to send the logs with `printf`.


## Custom log function
If you can't use `printf` or want to use a custom function to log, you can register a "logger" callback with `lv_log_register_print_cb()`. 

For example:

```c
void my_log_cb(lv_log_level_t level, const char * file, uint32_t line, const char * fn_name, const char * dsc)
{
  /*Send the logs via serial port*/
  if(level == LV_LOG_LEVEL_ERROR) serial_send("ERROR: ");
  if(level == LV_LOG_LEVEL_WARN)  serial_send("WARNING: ");
  if(level == LV_LOG_LEVEL_INFO)  serial_send("INFO: ");
  if(level == LV_LOG_LEVEL_TRACE) serial_send("TRACE: ");

  serial_send("File: ");
  serial_send(file);

  char line_str[8];
  sprintf(line_str,"%d", line);
  serial_send("#");
  serial_send(line_str);

  serial_send(": ");
  serial_send(fn_name);
  serial_send(": ");
  serial_send(dsc);
  serial_send("\n");
}

...


lv_log_register_print_cb(my_log_cb);

```

## Add logs

You can also use the log module via the `LV_LOG_TRACE/INFO/WARN/ERROR(description)` functions.
