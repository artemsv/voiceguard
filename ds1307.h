//  24.09.2006

typedef struct	tagRTC_TIME
	{
	unsigned char	sec;
	unsigned char	min;
	unsigned char	hour;
	unsigned char	weekday;
	unsigned char	day;
	unsigned char	month;
	unsigned char	year;

} RTC_TIME, *PRTC_TIME;

void InitRTC(void);
void ReadTime(PRTC_TIME prtc);
void UpdateTimePane(void);
void WriteTime(PRTC_TIME prtc);
