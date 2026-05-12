% main_roberts.m
% ============================================================
%  DETEKSI TEPI OPERATOR ROBERTS - SCRIPT UTAMA
%  Mata Kuliah: Pengolahan Citra Digital (GKV - IF 2020)
% ============================================================
%
%  File yang diperlukan di folder yang sama:
%    konvolusi2x2.m   thresholding.m   roberts.m   kucing.jpeg
%
%  Cara menjalankan di Octave/MATLAB:
%    >> main_roberts
% ============================================================

clc; clear; close all;

fprintf('============================================\n');
fprintf('  DETEKSI TEPI - OPERATOR ROBERTS (2x2)\n');
fprintf('============================================\n\n');

%% ── BACA CITRA ─────────────────────────────────────────────
citra_raw = imread('kucing.jpeg');

% Konversi ke grayscale jika RGB
if size(citra_raw, 3) == 3
    citra = rgb2gray(citra_raw);
else
    citra = citra_raw;
end

[H, W] = size(citra);
fprintf('[INFO] File   : kucing.jpeg\n');
fprintf('[INFO] Ukuran : %d x %d piksel\n\n', W, H);

%% ── KERNEL ROBERTS ─────────────────────────────────────────
% Ditampilkan agar sesuai materi kuliah
Rp_kernel = [ 1  0;  0 -1];    % diagonal /
Rm_kernel = [ 0  1; -1  0];    % diagonal \

fprintf('Kernel R+ (diagonal /):\n');
disp(Rp_kernel);
fprintf('Kernel R- (diagonal \\):\n');
disp(Rm_kernel);

%% ── DETEKSI TEPI DENGAN 3 NILAI AMBANG ─────────────────────
% Percobaan dengan T = 64, 128, 192 untuk melihat perbedaan
ambang_list = [64, 128, 192];
label_list  = {'T = 64  (sensitif)', ...
               'T = 128 (standar)', ...
               'T = 192 (ketat)'};

for k = 1:3
    T = ambang_list(k);
    fprintf('[%d] Memproses dengan ambang T = %d ...\n', k, T);

    [hasil, magnitude, Rp, Rm] = roberts(citra, T);

    % Simpan hasil
    nama_file = sprintf('hasil_roberts_T%d.png', T);
    imwrite(hasil, nama_file);
    fprintf('    Disimpan -> %s\n', nama_file);

    % Tampilkan
    figure(k);
    subplot(2, 3, 1);
    imshow(citra);
    title('Citra Asal (Grayscale)', 'FontSize', 10, 'FontWeight', 'bold');

    subplot(2, 3, 2);
    imshow(uint8(abs(Rp) / max(abs(Rp(:))) * 255));
    title('Respons R+ (diagonal /)', 'FontSize', 10);

    subplot(2, 3, 3);
    imshow(uint8(abs(Rm) / max(abs(Rm(:))) * 255));
    title('Respons R- (diagonal \)', 'FontSize', 10);

    subplot(2, 3, [4 5]);
    imshow(uint8(magnitude / max(magnitude(:)) * 255));
    title('Magnitude Tepi (|R+| + |R-|)', 'FontSize', 10);

    subplot(2, 3, 6);
    imshow(hasil);
    title(label_list{k}, 'FontSize', 10, 'FontWeight', 'bold');

    % Judul figure — cetak ke konsol (Octave tidak punya sgtitle)
    fprintf('    Figure %d: Roberts Edge Detection - T = %d\n', k, T);
end

%% ── PERBANDINGAN KETIGA AMBANG DALAM SATU FIGURE ───────────
figure(4);
subplot(1, 4, 1);
imshow(citra);
title('Asal', 'FontSize', 11, 'FontWeight', 'bold');

for k = 1:3
    T = ambang_list(k);
    [hasil_cmp, ~, ~, ~] = roberts(citra, T);
    subplot(1, 4, k+1);
    imshow(hasil_cmp);
    title(sprintf('T = %d', T), 'FontSize', 11);
end
fprintf('Figure 4: Perbandingan Nilai Ambang - Roberts\n');

fprintf('\n============================================\n');
fprintf('  Selesai! Hasil disimpan sebagai PNG.\n');
fprintf('============================================\n');
