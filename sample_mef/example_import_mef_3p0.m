% EXAMPLE_IMPORT_MEF_3P0 an example to import MEF 3.0 data
% 
% Syntax:
%   example_import_mef_3p0
% 
% See also example_import_mef_2p1.

% Copyright 2020 Richard J. Cui. Adapted: Wed 02/12/2020  3:31:26.134 PM
% $Revision: 0.2 $  $Date: Thu 02/20/2020 12:11:06.383 PM $
%
% 1026 Rocky Creek Dr NE
% Rochester, MN 55906, USA
%
% Email: richard.cui@utoronto.ca

% Set MEF version
% ---------------
mef_ver = 3.0;
 
% Set the session path
% --------------------
sample_data_folder = fileparts(mfilename("fullpath"));
sess_path = fullfile(sample_data_folder, 'mef_3p0.mefd');

% select channels
% ---------------
% the type of the variable of the selected channels is string array
sel_chan = []; % all channels
% sel_chan = ["left_central-ref", "Left_Occipital-Ref",...
%     "Left-Right_Central", "left-right_occipital"]; 

% set the start and end time point of signal segment
% --------------------------------------------------
% this is a relative time point. the time of signal starts at 0 and the 1st
% sample index is 1.
start_end = []; % all data
% start_end = [0, 10]; 

% set the unit of time point 
% --------------------------
% in this example, we will import the signal from 0 second to 10 second
unit = 'second'; 

% set the password structure for MEF 3.0 sample data
password = struct('Level1Password', 'password1', 'Level2Password',...
    'password2', 'AccessLevel', 2); 

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