# LCD console library

Emulate character console (display only).


## Function Reference

### void lcdcon_init( void );
initialize

### void lcdcon_clear( void );
clear all

### void lcdcon_locate( int row, int column );
set display position

`lcdcon_locate( 1, 5 );`

### void lcdcon_putc( int ch );
put a character

`lcdcon_putc('A');`

### void lcdcon_puts( const char*s );
put a string

`lcdcon_puts("HELLO WORLD.¥n");`

### void lcdcon_scroll( int row );
scroll

`lcdcon_scroll(-1);   // back -3 lines.`


## Notes

スクロール中に新規文字表示

スクロール中に表示を行う事はサポートしていない。
この規模（小規模）のライブラリで、スピード劣化やサイズの肥大化を（それほど）招かずに、スクロールと表示を同時にサポートするのはどうすべきか、ケースバイケースで仕様を決めかねる。

（案1）
スクロール中は表示停止、表示タスクは強制的に停止されられる。
簡単だが、OSとの連携が必要なのでで、このライブラリへの組込みは不可能。

（案2）
スクロール中も内部キャラクタバッファに書き込み。都度リフレッシュ表示。
一文字表示ごとに、スクロール中か？　表示エリア内か？　などの判定が必要で、CPUコストが高い。
スクロール中にバッファがあふれたら、強制的にスクロールをあふれ分戻す処理が必要になるなど、コストが高い割に、利用者にとって若干不自然な点も残してしまう。
