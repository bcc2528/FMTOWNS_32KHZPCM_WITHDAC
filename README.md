# FM TOWNSで32KHz PCM再生

FM TOWNSには8bit 20.833KHz(カタログスペックでは19.2KHz)のPCM音源が搭載されているが、YM2612のDACを使って20.833KHzを超す32KHzのPCM再生を行うデモプログラムです。

DAC_PCM.EXPとDAC.PCMを同一フォルダに入れてDAC_PCM.EXPを実行。以下25秒で永久ループ再生。PCMファイルが無い場合は400Hzの鋸波が延々と再生されます。

-DAC.PCMの形式  
現時点でサンプルは作ってないので、各自で作る必要あり  
* ヘッダーはなし、、32KHz、サイズは25秒再生で丁度800KBまで)
* モノラル
* 符号なし8bit : -1.0が0x0, 0.0が0x7f, +1.0が0xffとなるようにする。WAVE 16bitから変更する場合は、32768を足して8ビット右シフトした値。
* 32KHz サンプリングレート
* 長さは25秒まで(上記の形式も合わせるとファイルサイズは800KB丁度となる)
  

実機FM TOWNS マーティーで動作確認。486 CPU TOWNSでどうなるかは未確認。エミュレータなどではYM2612のDACとBUSYレジスタを再現しているものが現時点でないと思うので再生不可。

たけがみりう氏作X68PPLAY.EXPのソースコードを参考にしました
