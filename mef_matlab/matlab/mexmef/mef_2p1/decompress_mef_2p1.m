function data = decompress_mef_2p1(wholename,start_index,stop_index,password)
% DECOMPRESS_MEF_2P1 Decompress MEF 2.1 data blocks
% 
% Syntax:
%   data = decompress_mef_2p1(wholename, start_index, stop_index, password)
% 
% Imput(s):
%   wholename       - [str] fullpath of MEF file
%   start_index     - [scalar] sample start index (>= 0)
%   stop_index      - [scalar] sample stop index (>= start)
%   password        - [str] password of the data
% 
% Output(s):
%   data            - [int32 array] decompressed signal
% 
% Note:
%   This is a dummy function to check if the mex function has been
%   compiled. If not, it will try to compile it.
% 
% See also multiscaleelectrophysiologyfile_2p1.importsignal.

% Copyright 2019-2020 Richard J. Cui. Created: Mon 04/29/2019 10:33:58.517 PM
% $Revision: 0.4 $  $Date: Mon 11/02/2020  4:27:10.491 PM $
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
data = decompress_mef_2p1(wholename,start_index,stop_index,password);

end % function

% [EOF]