% EXAMPLE_IMPORT_MEF_2P1 an example to import MEF 2.1 data
% 
% Syntax:
%   example_import_mef_2p1
% 
% See also example_import_mef_3p0.

% Copyright 2020 Richard J. Cui. Adapted: Wed 02/12/2020  3:31:26.134 PM
% $Revision: 0.1 $  $Date: Wed 02/12/2020  3:31:26.134 PM $
%
% 1026 Rocky Creek Dr NE
% Rochester, MN 55906, USA
%
% Email: richard.cui@utoronto.ca

% Set MEF version
% ---------------
mef_ver = 2.1;
 
% Set the session path
% --------------------
sample_data_folder = fileparts(mfilename("fullpath"));
sess_path = fullfile(sample_data_folder, 'mef_2p1');

% select channels
% ---------------
% the type of the variable of the selected channels is string array
sel_chan = ["B_1", "D_1", "F_1", "S_1"]; 

% set the start and end time point of signal segment
% --------------------------------------------------
% this is a relative time point. the time of signal starts at 0 and the 1st
% sample index is 1.
start_end = [0, 10]; 

% set the unit of time point 
% --------------------------
% in this example, we will import the signal from 0 second to 10 second
unit = 'second'; 

% set the password structure for MEF 3.0 sample data
password = struct('Subject', 'erlichda', 'Session', 'sieve', 'Data', ''); 

% import the signal into EEGLAB
% -----------------------------
% the variable 'EEG' is set by EEGLAB
% or you may create an empty one by using command 'EEG = eeg_emptyset();'
if ~exist('EEG', 'var'), EEG = eeg_emptyset; end % if
EEG = pop_mefimport(EEG, mef_ver, sess_path, sel_chan, start_end,...
    unit, password); 

% plot the signal
% ---------------
pop_eegplot_w(EEG, 1, 0, 1); 

% [EOF]