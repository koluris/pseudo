class CstrMotionDecoder {
    uw cmd, status;
    
public:
    void reset();
    void write(uw, uw);
    uw read(uw);
};

extern CstrMotionDecoder mdec;
