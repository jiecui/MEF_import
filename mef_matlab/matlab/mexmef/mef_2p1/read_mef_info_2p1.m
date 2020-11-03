function header = read_mef_info_2p1(wholename,password)
% READ_MEF_INFO_2P1 Read header information from MEF 2.1 dataset
% 
% Syntax:
%   header = read_mef_info_2p1(wholename, pw)
% 
% Imput(s):
%   wholename       - [str] filepath + filename of MEF file
%   password        - [str] password of the data
% 
% Output(s):
%   header          - [struc] HEADER information of MEF 2.1 file
% 
% Note:
%   This is a dummy function to check if the mex function has been
%   compiled. If not, it will try to compile it.
% 
% See also multiscaleelectrophysiologyfile_2p1.readheader.

% Copyright 2020 Richard J. Cui. Created: Mon 11/02/2020  4:27:10.491 PM
% $Revision: 0.1 $  $Date: Mon 11/02/2020  4:27:10.491 PM $
%
% Rocky Creek Dr NE
% Rochester, MN 55906, USA
%
% Email: richard.cui@utoronto.ca

% compile c-mex function
% -----------------------
% we are here, cuz we don't have the mex function compiled. So, do it now
make_mex_mef

% now get the info
% ----------------
header = read_mef_info_2p1(wholename,password);

end % function

% [EOF]