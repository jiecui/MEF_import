function metadata = read_mef_info_3p0(sess_path,password,map_indices)
% READ_MEF_INFO_3P0 Read metadata information from MEF 3.0 dataset
% 
% Syntax:
%   metadata = read_mef_info_3p0(sess_path,password,map_indices)
% 
% Imput(s):
%   sess_path       - [str] session path
%   password        - [struct] password structure of MEF 3.0
%                     .Level1Password
%                     .Level2Password
%                     .AccessLevel
%   map_indices     - [logical] 'ture' means to map indices
% 
% Output(s):
%   metadata        - [struct] MEF 3.0 metadata structure
% 
% Note:
%   This is a dummy function to check if the mex function has been
%   compiled. If not, it will try to compile it.
% 
% See also mefsession_3p0.read_mef_info.

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

% now get the info
% ----------------
metadata = read_mef_info_3p0(sess_path,password,map_indices);

end % funciton

% [EOF]