'''
Generates a chechker file for lv_conf.h from lv_conf_templ.h define all the not defined values
'''


import re

fin = open("../lv_conf_template.h", "r");
fout = open("../src/lv_conf_checker.h", "w");


fout.write(
'/**\n\
 * GENERATED FILE, DO NOT EDIT IT!\n\
 * @file lv_conf_checker.h\n\
 * Make sure all the defines of lv_conf.h have a default value\n\
**/\n\
\n\
#ifndef LV_CONF_CHECKER_H\n\
#define  LV_CONF_CHECKER_H\n\
'
)  

inlines = fin.read().splitlines();

started = 0

for i in inlines:
  if(not started):
    if('#define LV_CONF_H' in i):
      started = 1 
      continue
    else:
      continue
    
  if('/*--END OF LV_CONF_H--*/' in i): break  
    
  if(re.search('^ *# *define .*$', i)): 
    new = re.sub('^ *# *define', '#define ', i)       
    new = re.sub(' +', ' ', new)                 #Remove extra white spaces
    splitted = new.split(' ')
    fout.write('#ifndef ' + splitted[1] + '\n')
    fout.write(i + '\n') 
    fout.write('#endif\n')
  elif(re.search('^ *typedef .*;.*$', i)):
    continue;   #igonre typedefs to avoide redeclaration
  else:
    fout.write(i + '\n')  
    
    
fout.write(
'\n\
#endif  /*LV_CONF_CHECKER_H*/\n\
')
