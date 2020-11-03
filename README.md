An EEGLAB Plugin of MEF Dataset (Ver 1.20)
==========================================

[![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/jiecui/MEF_import)](https://github.com/jiecui/MEF_import/releases/tag/v1.19)
[![HitCount](http://hits.dwyl.io/jiecui/MEF_import.svg)](http://hits.dwyl.io/jiecui/MEF_import)

Introduction
------------
**MEF_import** is an [EEGLAB](https://sccn.ucsd.edu/eeglab/index.php) plugin that imports data compressed in Multiscale Electrophysiology Format (or Mayo EEG File, MEF, see below) and Multiscale Annotation File (MAF) data into EEGLAB.
Current version can import [MEF/MAF Version 2.1](https://github.com/benbrinkmann/mef_lib_2_1) and [MEF version 3.0](https://msel.mayo.edu/codes.html) files.
Moreover, the functions provided in the folder "mef_matlab" can be used as a general tool to import MEF data into MATLAB.

The code repository for **MEF_import** is hosted on GitHub at https://github.com/jiecui/MEF_import.

Installation
------------
1. Download, decompress and copy the directory into the directory of plugins of EEGLAB (/root/directory/of/eeglab/plugins)
1. Rename the folder name of MEF_import plugin to MEF_import1.20
1. Alternatively, you may prefer to install the plugin as a Git submodule: go to the directory of eeglab, then run 

        git submodule add https://github.com/jiecui/MEF_import ./plugins/MEF_import

   subsequently, change the folder name of the plugin 

        git mv ./plugins/MEF_import ./plugins/MEF_import1.20.

   On the other hand, if you have already installed it as a submodule, say MEF_import1.19, use ```git mv``` to rename it to MEF_import1.20 and then pull the changes from the remote.
1. Launch EEGLAB in MATLAB, e.g. ```>> eeglab```
1. Follow the instructions on the screen

Mex file
--------
Several mex files are required to read MEF data.
The c-mex functions will be automatically compiled the first time runing the code.
Alternatively, after launch EEGLAB, run 'make_mex_mef.m' to build the mex files for different operating systems.
 
Data samples
------------
1. Two small sample datasets, of MEF 2.1 and MEF 3.0, respectively, are provided to test the code.
1. The folder of the sample datasets is 'sample_mef' (/root/directory/of/eeglab/plugins/MEF_import1.20/sample_mef).
Two data samples are included as subdirectories: 'mef_2p1' and 'mef_3p0.mefd'.
1. The directory 'mef_2p1' includes the session of MEF 2.1 signal (passwords: 'erlichda' for Subject password; 'sieve' for Session password; no password required for Data password).
1. The directory 'mef_3p0' includes the session of MEF 3.0 signal (level 1 password: password1; level 2 passowrd: password2; access level: 2).

Input MEF data
--------------
*Input signal using GUI*

1. From EEGLAB GUI, select File > Import Data > Using EEGLAB functions and plugins > From Mayo Clinic .mef. 
Then choose 'MEF 2.1' to import MEF 2.1 format data, or choose 'MEF 3.0' to import MEF 3.0 data.
1. If passwords are required, click "Set Passwords" to input the passwords.
1. Select the folder of the dataset.  A list of available channel is shown in the table below.
1. Choose part of the signal to import if needed.
1. Discontinuity of recording is marked as event 'Discont'.

*Input signal using MATLAB commandline*

The following code is an example to import a segment of MEF 3.0 signal into MATLAB/EEGLAB and plot it (after launch EEGLab):

```matlab
% set MEF version
% ---------------
mef_ver = 3.0; 

% set the session path
% --------------------
% please replace the root directory of eeglab with the directory on your system
sess_path = '/root/directory/of/eeglab/plugins/MEF_import1.20/sample_mef/mef_3p0.mefd';

% select channels
% ---------------
% the type of the variable of the selected channels is string array
sel_chan = ["left_central-ref", "Left_Occipital-Ref", "Left-Right_Central", "left-right_occipital"]; 

% set the unit of time point 
% --------------------------
% in this example, we will import the signal from 0 second to 10 second
unit = 'second'; 

% set the start and end time point of signal segment
% --------------------------------------------------
% this is a relative time point. the time of signal starts at 0 and the 1st sample index is 1.
start_end = [0, 10]; 

% set the password structure for MEF 3.0 sample data
% --------------------------------------------------
password = struct('Level1Password', 'password1', 'Level2Password', 'password2', 'AccessLevel', 2); 

% import the signal into EEGLAB
% -----------------------------
% the variable 'EEG' is set by EEGLAB
% or you may create an empty one by using command 'EEG = eeg_emptyset();'
EEG = pop_mefimport(EEG, mef_ver, sess_path, sel_chan, start_end, unit, password); 

% plot the signal
% ---------------
pop_eegplot_w(EEG, 1, 0, 1); 
```
You may repeat the result by running the included script *example_import_mef_3p0.m*.
You can import the data encoded with MEF 2.1 format, an older version of MEF, similarly. 
An example to import MEF 2.1 data using commandline, *example_import_mef_2p1.m*, is also included.

Input MAF data
--------------
From EEGLAB GUI, select File > Import event info > From Mayo Clinic .maf > MEF 2.1

Currently, the importer can only recognize events of seizure onset and seizure offset.

Multiscale Electrophysiology Format (MEF)
-----------------------------------------
MEF is a novel electrophysiology file format designed for large-scale storage of electrophysiological recordings.
MEF can achieve significant data size reduction when compared to existing techniques with stat-of-the-art lossless data compression.
It satisfies the Health Insurance Portability and Accountability Act (HIPAA) to encrypt any patient protected health information transmitted over a public network.

The details of MEF file can be found at [here](https://www.mayo.edu/research/labs/epilepsy-neurophysiology/mef-example-source-code) from [Mayo Systems Electrophysiology Lab](http://msel.mayo.edu/) and [here](https://main.ieeg.org/?q=node/28) on [International Epilepsy Portal](https://main.ieeg.org). 

License
-------
**MEF_import** is distributed with the GPL v3 Open Source License.
