function data = decompress_mef_3p0(ch_path,pw,rtype,begin,stop)
% decompress_mef_3p0 Read data for a single channle of MEF 3.0 session
% 
% Syntax:
%   data = decompress_mef_3p0(ch_path,pw,rtype,begin,stop)
% 
% Imput(s):
%   ch_pass         - [str] channel path of a MEF 3.0 session
%   pw              - [str] password for the desired level
%   rtype           - [str] range type: the unit of the range of the data
%                     to be read ('samples', 'time')
%   begin           - [num] begin point
%   stop            - [num] stop point
% 
% Output(s):
%   data            - [array] channel data
% 
% Note:
%   This is a dummy function to check if the mex function has been
%   compiled. If not, it will try to compile it.
% 
% See also multiscaleelectrophysiologyfile_3p0.read_mef_data.

% Copyright 2020 Richard J. Cui. Created: Mon 11/02/2020  3:44:14.289 PM
% $Revision: 0.1 $  $Date: Mon 11/02/2020  3:44:14.289 PM $
%
% Rocky Creek Dr NE
% Rochester, MN 55906, USA
%
% Email: richard.cui@utoronto.ca

% compile c-mex function
% -----------------------
% we are here, cuz we don't have the mex function compiled. So, do it now
make_mex_mef

% now get the data
% ----------------
data = decompress_mef_3p0(ch_path,pw,rtype,begin,stop);

end % funciton

% [EOF]