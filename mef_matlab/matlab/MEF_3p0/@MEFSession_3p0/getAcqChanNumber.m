function ac_num = getAcqChanNumber(this)
% MEFSESSION_3P0.GETACQCHANNUMBER get the number of acquisition channel
%
% Syntax:
%   ac_num = getAcqChanNumber(this)
% 
% Input(s):
%   this            - [obj] MEFSession_3p0 object
% 
% Output(s):
%
% Example:
%
% Note:
%
% References:
%
% See also .

% Copyright 2020 Richard J. Cui. Created: Fri 04/10/2020  9:57:42.678 AM
% $Revision: 0.2 $  $Date: Wed 09/23/2020  4:17:22.912 PM $
%
% Rocky Creek Dr NE
% Rochester, MN 55906, USA
%
% Email: richard.cui@utoronto.ca

% =========================================================================
% parse inputs
% =========================================================================
q = parseInputs(this);
this = q.this;

% =========================================================================
% Main
% =========================================================================
metadata = this.MetaData;
n_chan = metadata.number_of_time_series_channels;
ac_num = zeros(1, n_chan);
for k = 1:n_chan
    ac_num(k) = metadata.time_series_channels(k).metadata.section_2.acquisition_channel_number;
end % for

end % function getAcqChanNumber

% =========================================================================
% Subroutines
% =========================================================================
function q = parseInputs(varargin)

% default

% parse rules
p = inputParser;
p.addRequired('this', @isobject)

% parse and return
p.parse(varargin{:});
q = p.Results;

end % function

% [EOF]