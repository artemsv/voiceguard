program IncBuildNo;

{$APPTYPE CONSOLE}

uses
  SysUtils;

var
	f: text;
	st1, st: string;
	verno: integer;
begin
	Assign(f, 'F:\Projects\VoiceGuard\VG\SRC\BuildNo.inc');
	Reset(f);
	Readln(f, st);
	st1 := Copy(st, 31, 4);
	verno := StrToInt(st1);
	Inc(verno);
	Rewrite(f);
	Write(f, 'unsigned char* szBuildNo = "1.');
	st := IntToStr(verno);
	while Length(st) < 4 do
		st := '0' + st;

	Writeln('BuildNO = ', st);
	Write(f, st);
	Writeln(f, '";');
	Close(f);
end.
 