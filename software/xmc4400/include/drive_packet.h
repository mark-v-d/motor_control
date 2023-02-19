namespace interface {

struct  __attribute__ ((__packed__)) from_drive {
    int32_t position;
    float angle;
    int32_t valid;
    float Irotor[2];
    float Vrotor[2];
};

struct  __attribute__ ((__packed__)) to_drive {
    float Iset[2];
};

}
