

const int fpt = 1 << 14;
#define tofixed(n) ((n)*fpt)
#define toint(x) ((x)/fpt)
#define toint_r(x) ((x)>=0?((x)+(fpt>>1)/fpt):((x)-(fpt>>1)/fpt))
#define addfixed(x,y) ((x)+(y))
#define subfixed(x,y) ((x)-(y))
#define addcompound(x,n) ((x)+(n)*fpt)
#define subcompound(x,n) ((x)+(n)*fpt)
#define multfixed(x,y) ((int64_t)(x)*(y)/fpt)
#define multcompound(x,n) ((x)*(n))
#define divfixed(x,y) ((int64_t)(x)*(fpt)/(y))
#define divcompound(x,n) ((x)/(n))

