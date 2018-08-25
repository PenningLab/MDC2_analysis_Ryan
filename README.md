# MDC2_analysis_Ryan
The Calibration_macro handles the analysis of calibration data of MDC2

To compile :

source setup_pdsf.sh

g++ -I /usr/common/usg/software/ROOT/6.04.00/include/ `root-config --cflags --libs` -o Calibraton_macro Calibraton_macro.cc rqlib/rqlib.so

To execute : 
One can either compile their own list of filenames from MDC2 or simply type what kind of calibration source at waht position (currently only 700mm is impletmented), and the desired number of root files. Then the code will automatically link these files from MDC2 data directory into TChain.

Usage : ./Calibraton_macro [-co] ./Calibraton_macro Options:
 -list : load the list of filename 
 -type : currently one can choose : AmLi700, Na22, background 
 -debug : debug mode 
 -h or --help : Show the usage
