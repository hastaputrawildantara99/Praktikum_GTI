% roberts.m
% ============================================================
%  DETEKSI TEPI - OPERATOR ROBERTS (kernel 2x2)
% ============================================================
%
%  Rumus:
%    R+(x,y) = f(x+1, y+1) - f(x, y)     → diagonal /
%    R-(x,y) = f(x,  y+1) - f(x+1, y)    → diagonal \
%
%  Kernel:
%    R+ = [ 1  0]      R- = [ 0  1]
%         [ 0 -1]           [-1  0]
%
%  Magnitute Tepi:
%    G[f(x,y)] = |R+| + |R-|
%
%  Kemudian dilakukan thresholding dengan nilai ambang T.
%
% PARAMETER:
%   citra - matriks grayscale uint8
%   T     - nilai ambang (0-255), default 128
%
% OUTPUT:
%   hasil     - citra biner hasil deteksi tepi (uint8)
%   magnitude - peta kekuatan tepi sebelum thresholding
%   Rp        - respons kernel R+ (diagonal /)
%   Rm        - respons kernel R- (diagonal \)
%
% Contoh:
%   hasil = roberts(citra, 128);
%   [hasil, mag] = roberts(citra, 100);
% ============================================================

function [hasil, magnitude, Rp, Rm] = roberts(citra, T)
    if nargin < 2
        T = 128;     % nilai ambang default
    end

    % ── Kernel Roberts 2x2 ──────────────────────────────────
    % R+ mendeteksi tepi diagonal kiri-bawah ke kanan-atas (/)
    kernel_Rp = [ 1  0; ...
                  0 -1];

    % R- mendeteksi tepi diagonal kanan-bawah ke kiri-atas (\)
    kernel_Rm = [ 0  1; ...
                 -1  0];

    % ── Konvolusi citra dengan kedua kernel ─────────────────
    Rp = konvolusi2x2(citra, kernel_Rp);   % respons diagonal /
    Rm = konvolusi2x2(citra, kernel_Rm);   % respons diagonal \

    % ── Hitung magnitude tepi: G = |R+| + |R-| ──────────────
    magnitude = abs(Rp) + abs(Rm);

    % ── Thresholding untuk menghasilkan citra biner ─────────
    hasil = thresholding(magnitude, T);
end
