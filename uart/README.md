# UART wrapper for PSoC5LP

## PSoC Creatorの設定

- Communication > UARTデバイス配置
 - ConfigureダイアログでNameを"UART1"に変更
 - 通信速度等のパラメータ設定
 - 同ダイアログAdvancedタブのInterruptを、"RX- On Byte Received"と"TX - On TX Complete"にチェックを入れる
- System > Interrupt デバイスを2つ配置
 - UARTのtx_interruptとrx_interruptに接続
 - 名前をそれぞれ、"isr_UART1_Tx"と、"isr_UART1_Rx"に変更

## ライブラリ使用

### 初期化
UART_HANDLER uh;
uart_init( &uh );

### 文字列送信
int txlen = uart_puts( &uh, "String" );

### 文字列受信
char buf[32];
rxlen = uart_gets( &uh, buf, sizeof(buf) );

※バイナリ送受信も可能、ソースコード参照。
