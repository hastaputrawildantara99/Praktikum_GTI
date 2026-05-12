% konvolusi2x2.m
% Konvolusi manual untuk kernel 2x2 (digunakan oleh operator Roberts)
function hasil = konvolusi2x2(citra, kernel)
    [H, W] = size(citra);
    citra_d = double(citra);
    hasil   = zeros(H, W);

    % Iterasi hanya sampai H-1 dan W-1 karena kernel 2x2
    for y = 1:H-1
        for x = 1:W-1
            % Ambil region 2x2
            region = citra_d(y:y+1, x:x+1);
            % Jumlahkan hasil perkalian element-wise
            hasil(y, x) = sum(sum(region .* kernel));
        end
    end
end
