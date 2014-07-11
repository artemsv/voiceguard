unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ExtCtrls, TeeProcs, TeEngine, Chart, Series, StdCtrls;

type
  TForm1 = class(TForm)
    Chart1: TChart;
    Series1: TFastLineSeries;
    Button1: TButton;
    procedure Button1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}


var
	s: array[0..92] of integer = (308, 308, 311, 315, 318, 326, 332, 342, 350,
	361, 372, 386, 397, 413, 425, 442, 455, 473, 487, 506, 519, 538, 551, 569, 582,
	600, 612, 628, 639, 652, 662, 674, 682, 692, 697, 704, 709, 712, 715, 715, 715,
	714, 711, 706, 702, 694, 688, 677, 669, 657, 646, 632, 621, 604, 592, 575, 561,
	543, 530, 511, 497, 479, 465, 447, 434, 418, 405, 390, 379, 366, 357, 344, 338,
	329, 322, 317, 313, 319, 324, 332, 339, 349, 358, 371, 381, 396, 408, 424, 437,
	454, 468, 486, 500
	);
procedure TForm1.Button1Click(Sender: TObject);
var
	ls: TLineSeries;
	k: integer;
begin
	ls := TLineSeries.Create(self);

	for k := 0 to 92 do
		ls.AddXY(k, s[k]);

	chart1.AddSeries(ls);
								{
	ls := TLineSeries.Create(self);

	for k := 0 to 350 do
		ls.AddXY(k, 700*sin(k));
	chart1.AddSeries(ls);
                 }
end;

end.
