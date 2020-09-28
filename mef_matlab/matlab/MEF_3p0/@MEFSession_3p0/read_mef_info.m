function metadata = read_mef_info(this, varargin)
% MEFSESSION_3P0.READ_MEF_INFO Retrieves metadata info from MEF 3.0 session
%   
% Syntax:
%   metadata = read_mef_info(this)
%   metadata = __(__, sess_path)
%   metadata = __(__, sess_path, password)
%   metadata = __(__, sess_path, password, map_indices)
% 
% Input(s):
%   this            - [obj] MEFSession_3p0 object
%   sess_path     	- [str] (opt) path (absolute or relative) to the MEF3 
%                     session folder (default = path in this)
%   password        - [struct] password structure to the MEF3 data (default
%                     = password in this)
%   map_indices     - [logical] flag whether indices should be mapped [true
%                     or false] (default = true)
%
% Output(s): 
%   metadata    	- [struct] structure containing session metadata, 
%                     channels metadata, segments metadata and records
%
% Note:
% 
% See also read_mef_info_3p0.

% Copyright 2020 Richard J. Cui. Adapted: Sat 02/01/2020 10:30:50.708 PM
% $Revision: 0.7 $  $Date: Fri 09/25/2020  9:41:56.944 AM $
%
% Rocky Creek Dr NE
% Rochester, MN 55906, USA
%
% Email: richard.cui@utoronto.ca

% =========================================================================
% parse inputs
% =========================================================================
q = parseInputs(this, varargin{:});
sess_path = q.sess_path;
password = q.password;
map_indices = q.map_indices;

if isempty(sess_path)
    sess_path = this.SessionPath;
end % if

if isempty(password)
    password = this.Password;
end % if

% =========================================================================
% main
% =========================================================================
pw = this.processPassword(password);
metadata = read_mef_info_3p0(sess_path, pw, map_indices); % mex

end % function

% =========================================================================
% subroutines
% =========================================================================
function q = parseInputs(varargin)

% defaults
default_sp = ''; % sesseion path
default_pw = struct([]);
default_mi = true;

% parse rules
p = inputParser;
p.addRequired('this', @isobject);
p.addOptional('sess_path', default_sp, @isstr);
p.addOptional('password', default_pw, @isstruct);
p.addOptional('map_indices', default_mi, @islogical);

% parse inputs and return results
p.parse(varargin{:});
q = p.Results;

end % function

% [EOF]