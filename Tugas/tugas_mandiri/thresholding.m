% thresholding.m
% Binarisasi citra berdasarkan nilai ambang T
% Piksel >= T → 255 (putih/tepi), piksel < T → 0 (hitam)
function hasil = thresholding(citra, T)
    [H, W] = size(citra);
    hasil  = zeros(H, W, 'uint8');

    for y = 1:H
        for x = 1:W
            if citra(y, x) >= T
                hasil(y, x) = 255;
            else
                hasil(y, x) = 0;
            end
        end
    end
end
