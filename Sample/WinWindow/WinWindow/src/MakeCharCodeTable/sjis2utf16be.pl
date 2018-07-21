#!/usr/local/bin/perl

use utf8;
use Encode;

# デバッグ制御フラグ (0:OFF)
$dbg = 0;

# JISコードからUnicodeへの変換テーブルの作成
&init_jis2uni_1();
if ($dbg) {
	# 変換テーブルの内容の確認
	foreach $cj (keys %tbl_jis2uni_1) {
		$dj = unpack("H2", $cj);
		$du = unpack("H4", $tbl_jis2uni_1{$cj});
		print "$dj  $du\n";
	}
}
&init_jis2uni_2();
if ($dbg) {
	# 変換テーブルの内容の確認
	foreach $cj (keys %tbl_jis2uni_2) {
		$dj = unpack("H4", $cj);
		$du = unpack("H4", $tbl_jis2uni_2{$cj});
		print "$dj  $du\n";
	}
}

# 出力ファイルのオープン
$out_open = ">sjis2utf16be.hpp";
if (!open(FILE_OUT, $out_open)) {
	printf("File open error.\n");
	exit(1);
}
binmode (FILE_OUT);

#------------------------------------------------------------------------------------------
# 半角文字(ローマ字,ASCII)テーブルを作成
print FILE_OUT "const std::uint16_t sjis2utf16be_0000_00FF[] = {\n\t";

$csta = 0x00;

for ($c1 = 0x00; $c1 <= 0xFF; $c1++) { # $c1はSJIS第1バイト
	# Unicodeへの変換
	($c1_u, $c2_u) = &jis2uni_1($c1);

	if (($c1_u == 0) && ($c2_u == 0)) {
		printf FILE_OUT "0x0000";
	}
	else {
		# Unicode(UTF16BE)の表示
		printf FILE_OUT "0x%02X%02X", $c1_u, $c2_u;
	}

	if ($c1 == 0x00) {
		# 最初の文字
		print FILE_OUT ", ";
	}
	elsif ($c1 == 0xFF) {
		# 最後の文字
		printf FILE_OUT ",\t//0x%02X-0x%02X\n", $csta, $c1;
	}
	else {
		if (($c1 & 0x0F) == 0x0F) {
			# 1行16文字まで
			printf FILE_OUT ",\t//0x%02X-0x%02X\n\t", $csta, $c1;

			$csta = $c1 + 0x01;
		}
		else {
			print FILE_OUT ", ";
		}
	}
}
print FILE_OUT "};\n";



print FILE_OUT "\n\n\n";



#------------------------------------------------------------------------------------------
# SJISとUTF16のリストを作成
for ($c1 = 0x21; $c1 <= 0x7E; $c1++) { # $c1は第1バイト（区）
	# 区番号の設定
	$ku = $c1 - 0x20;

	for ($c2 = 0x21; $c2 <= 0x7E; $c2++) { # $c2は第2バイト（点）
		# 点番号の設定
		$ten = $c2 - 0x20;

		# SJISコードの表示
		($c1_s, $c2_s) = &jis2sjis_2($c1, $c2);
		$sjis = ($c1_s << 8) | $c2_s;
		#printf "[%02X][%02X] c1_s:%02X c2_s:%02X c12_s:%04X\n", $c1, $c2, $c1_s, $c2_s, $sjis;

		# Unicodeへの変換
		($c1_u, $c2_u) = &jis2uni_2($c1, $c2);
		$utf16be = ($c1_u << 8) | $c2_u;
		#printf "[%02X][%02X] c1_u:%02X c2_u:%02X c12_u:%04X\n", $c1, $c2, $c1_u, $c2_u, $utf16be;
		printf "[%02X][%02X] sjis:0x%04X utf16le:0x%04X\n", $c1, $c2, $sjis, $utf16be;

		# SJIS->UTF16BEの一時リスト
		$jis2utf16be_tmp{$sjis} = $utf16be;
	}
}



#------------------------------------------------------------------------------------------
# 0x8100から0x9FFFまでの全角文字テーブルを作成
print FILE_OUT "const std::uint16_t sjis2utf16be_8100_9FFF[] = {\n\t";

$csta = 0x8100;

for ($sjis = 0x8100; $sjis <= 0x9FFF; $sjis++) {
	$utf = 0x0000;
	if (exists($jis2utf16be_tmp{$sjis})) {
		$utf = $jis2utf16be_tmp{$sjis};
	}

	# Unicode(UTF16BE)の表示
	printf FILE_OUT "0x%04X", $utf;

	if ($sjis == 0x8100) {
		# 最初の文字
		print FILE_OUT ", ";
	}
	elsif ($sjis == 0x9FFF) {
		# 最後の文字
		printf FILE_OUT ",\t//0x%02X-0x%02X\n", $csta, $sjis;
	}
	else {
		if (($sjis & 0x000F) == 0x000F) {
			# 1行16文字まで
			printf FILE_OUT ",\t//0x%02X-0x%02X\n\t", $csta, $sjis;

			$csta = $sjis + 0x0001;
		}
		else {
			print FILE_OUT ", ";
		}
	}
}
print FILE_OUT "};\n";



print FILE_OUT "\n\n\n";



#------------------------------------------------------------------------------------------
# 0xE000から0xFCFFまでの全角文字テーブルを作成
print FILE_OUT "const std::uint16_t sjis2utf16be_E000_FCFF[] = {\n\t";

$csta = 0xE000;

for ($sjis = 0xE000; $sjis <= 0xFCFF; $sjis++) {
	$utf = 0x0000;
	if (exists($jis2utf16be_tmp{$sjis})) {
		$utf = $jis2utf16be_tmp{$sjis};
	}

	# Unicode(UTF16BE)の表示
	printf FILE_OUT "0x%04X", $utf;

	if ($sjis == 0xE000) {
		# 最初の文字
		print FILE_OUT ", ";
	}
	elsif ($sjis == 0xFCFF) {
		# 最後の文字
		printf FILE_OUT ",\t//0x%02X-0x%02X\n", $csta, $sjis;
	}
	else {
		if (($sjis & 0x000F) == 0x000F) {
			# 1行16文字まで
			printf FILE_OUT ",\t//0x%02X-0x%02X\n\t", $csta, $sjis;

			$csta = $sjis + 0x0001;
		}
		else {
			print FILE_OUT ", ";
		}
	}
}
print FILE_OUT "};\n";

exit;



#------------------------------------------------------------------------------------------
# サブルーチン

# JISコードからUnicodeへの変換テーブルの作成
sub init_jis2uni_1 {
    my $cj; # SJIS char
    my $cu; # Unicode char
    my $s;  # string buffer

    # 変換テーブルのオープン
    $file_open = "<jis0201.txt";
    if (!open(FILE_IN, $file_open)) {
        printf("File open error.\n");
        exit(1);
    }

    while ($s = <FILE_IN>) {
        if ($s =~ /^#/) {next;} # 先頭が'#'の行を無視

        $cj = pack("H2", substr($s, 2, 2)); # JIS
        $cu = pack("H4", substr($s, 7, 4)); # Unicode

        $tbl_jis2uni_1{$cj} = $cu; # 変換テーブルの作成
    }

    # 変換テーブルのクローズ
    close(FILE_IN);
}

# JISコードからUnicodeへの変換テーブルの作成
sub init_jis2uni_2 {
    my $cj; # SJIS char
    my $cu; # Unicode char
    my $s;  # string buffer

    # 変換テーブルのオープン
    $file_open = "<jis0208.txt";
    if (!open(FILE_IN, $file_open)) {
        printf("File open error.\n");
        exit(1);
    }

    while ($s = <FILE_IN>) {
        if ($s =~ /^#/) {next;} # 先頭が'#'の行を無視

        $cj = pack("H4", substr($s, 9, 4));  # JIS
        $cu = pack("H4", substr($s, 16, 4)); # Unicode

        $tbl_jis2uni_2{$cj} = $cu; # 変換テーブルの作成
    }

    # 変換テーブルのクローズ
    close(FILE_IN);
}


# jis コードから sjis コードに変換
sub jis2sjis_1 {
    return($_[0] | 0x80, "");
}

# jis コードから sjis コードに変換
sub jis2sjis_2 {
    local($c1, $c2);

    # 1バイト目の変換
    if ($_[0] < 0x5f) { # 21-5E -> 81-9F
        $c1 = (($_[0] + 1) >> 1) + 0x70;
    } else { # 5F-7E -> E0-EF
        $c1 = (($_[0] + 1) >> 1) + 0xb0;
    }

    # 2バイト目の変換
    if ($_[0] % 2) { # 奇数区
        if ($_[1] < 0x60) { # 20-50 -> 3F-6F
            $c2 = $_[1] + 0x1f;
        } else { # 60-70 -> 80-90
            $c2 = $_[1] + 0x20;
        }
    } else { # 20-70 -> 9E-EE
        $c2 = $_[1] + 0x7e;
    }

    return($c1, $c2);
}


# JISコードからUnicodeへの変換
sub jis2uni_1 {
    local($sj, $su);

    $sj = chr($_[0]);
    $su = $tbl_jis2uni_1{$sj};

    return(ord(substr($su, 0, 1)), ord(substr($su, 1, 1)));
}

# JISコードからUnicodeへの変換
sub jis2uni_2 {
    local($sj, $su);

    $sj = chr($_[0]).chr($_[1]);
    $su = $tbl_jis2uni_2{$sj};

    return(ord(substr($su, 0, 1)), ord(substr($su, 1, 1)));
}


# UnicodeからUTF-8への変換
sub uni2utf8 {
    local ($c1, $c2, $c3);

    $n = $_[0] * 0x100 + $_[1];

    if ($n < 0x7F) { # 0000-007F
        $c1 = $n;
        return($c1);

    } elsif ($n < 0x800) { # 0080-0800
        $c1 = 0xC0 | ($n / 64);
        $c2 = 0x80 | ($n % 64);
        return($c1, $c2);

    } else { # 0800-FFFF
        $c1 = 0xE0 | (($n / 64) / 64);
        $c2 = 0x80 | (($n / 64) % 64);
        $c3 = 0x80 | ($n % 64);
        return($c1, $c2, $c3);
    }
}

1;
