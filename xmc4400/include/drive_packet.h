struct  __attribute__ ((__packed__)) output_t {
    uint32_t counter;
    int32_t position;
    int32_t position2;
    int32_t index2;
    float adc[4];
    float Irotor[2];
    float Vrotor[2];
    float I[2];
    float angle;
    float output[3];
    float vservo;
};

struct  __attribute__ ((__packed__)) input_t {
    float Iset[2];
    float kP[2], kI[2];
    float lim;
    float angle_offset;
};
