extern void (*psxCP2[64])(uw);

uw MFC2(int reg);
void MTC2(uw value, int reg);
void CTC2(uw value, int reg);

void psxNULL(uw code);
void gteRTPS(uw code);
void gteOP(uw code);
void gteNCLIP(uw code);
void gteDPCS(uw code);
void gteINTPL(uw code);
void gteMVMVA(uw code);
void gteNCDS(uw code);
void gteNCDT(uw code);
void gteCDP(uw code);
void gteNCCS(uw code);
void gteCC(uw code);
void gteNCS(uw code);
void gteNCT(uw code);
void gteSQR(uw code);
void gteDCPL(uw code);
void gteDPCT(uw code);
void gteAVSZ3(uw code);
void gteAVSZ4(uw code);
void gteRTPT(uw code);
void gteGPF(uw code);
void gteGPL(uw code);
void gteNCCT(uw code);
