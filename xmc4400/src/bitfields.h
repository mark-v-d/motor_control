namespace can_ns {
union id_t {
    struct {
        uint32_t mod_rev:8;	// 0
        uint32_t mod_type:8;	// 8
        uint32_t mod_number:16;	// 16
    };
    uint32_t raw;
};
}
namespace can_mo_ns {
union moamr_t {
    struct {
        uint32_t am:29;	// 0
        uint32_t mide:1;	// 29
    };
    uint32_t raw;
};
union moar_t {
    struct {
        uint32_t id:29;	// 0
        uint32_t ide:1;	// 29
        uint32_t pri:2;	// 30
    };
    uint32_t raw;
};
union moctr_t {
    struct {
        uint32_t resrxpnd:1;	// 0
        uint32_t restxpnd:1;	// 1
        uint32_t resrxupd:1;	// 2
        uint32_t resnewdat:1;	// 3
        uint32_t resmsglst:1;	// 4
        uint32_t resmsgval:1;	// 5
        uint32_t resrtsel:1;	// 6
        uint32_t resrxen:1;	// 7
        uint32_t restxrq:1;	// 8
        uint32_t restxen0:1;	// 9
        uint32_t restxen1:1;	// 10
        uint32_t resdir:1;	// 11
        uint32_t :4;
        uint32_t setrxpnd:1;	// 16
        uint32_t settxpnd:1;	// 17
        uint32_t setrxupd:1;	// 18
        uint32_t setnewdat:1;	// 19
        uint32_t setmsglst:1;	// 20
        uint32_t setmsgval:1;	// 21
        uint32_t setrtsel:1;	// 22
        uint32_t setrxen:1;	// 23
        uint32_t settxrq:1;	// 24
        uint32_t settxen0:1;	// 25
        uint32_t settxen1:1;	// 26
        uint32_t setdir:1;	// 27
    };
    uint32_t raw;
};
union modatah_t {
    struct {
        uint32_t db4:8;	// 0
        uint32_t db5:8;	// 8
        uint32_t db6:8;	// 16
        uint32_t db7:8;	// 24
    };
    uint32_t raw;
};
union modatal_t {
    struct {
        uint32_t db0:8;	// 0
        uint32_t db1:8;	// 8
        uint32_t db2:8;	// 16
        uint32_t db3:8;	// 24
    };
    uint32_t raw;
};
union mofcr_t {
    struct {
        uint32_t mmc:4;	// 0
        uint32_t :4;
        uint32_t gdfs:1;	// 8
        uint32_t idc:1;	// 9
        uint32_t dlcc:1;	// 10
        uint32_t datc:1;	// 11
        uint32_t :4;
        uint32_t rxie:1;	// 16
        uint32_t txie:1;	// 17
        uint32_t ovie:1;	// 18
        uint32_t :1;
        uint32_t frren:1;	// 20
        uint32_t rmm:1;	// 21
        uint32_t sdt:1;	// 22
        uint32_t stt:1;	// 23
        uint32_t dlc:4;	// 24
    };
    uint32_t raw;
};
union mofgpr_t {
    struct {
        uint32_t bot:8;	// 0
        uint32_t top:8;	// 8
        uint32_t cur:8;	// 16
        uint32_t sel:8;	// 24
    };
    uint32_t raw;
};
union moipr_t {
    struct {
        uint32_t rxinp:3;	// 0
        uint32_t :1;
        uint32_t txinp:3;	// 4
        uint32_t :1;
        uint32_t mpn:8;	// 8
        uint32_t cfcval:16;	// 16
    };
    uint32_t raw;
};
union mostat_t {
    struct {
        uint32_t rxpnd:1;	// 0
        uint32_t txpnd:1;	// 1
        uint32_t rxupd:1;	// 2
        uint32_t newdat:1;	// 3
        uint32_t msglst:1;	// 4
        uint32_t msgval:1;	// 5
        uint32_t rtsel:1;	// 6
        uint32_t rxen:1;	// 7
        uint32_t txrq:1;	// 8
        uint32_t txen0:1;	// 9
        uint32_t txen1:1;	// 10
        uint32_t dir:1;	// 11
        uint32_t list:4;	// 12
        uint32_t pprev:8;	// 16
        uint32_t pnext:8;	// 24
    };
    uint32_t raw;
};
}
namespace can_node_ns {
union nbtr_t {
    struct {
        uint32_t brp:6;	// 0
        uint32_t sjw:2;	// 6
        uint32_t tseg1:4;	// 8
        uint32_t tseg2:3;	// 12
        uint32_t div8:1;	// 15
    };
    uint32_t raw;
};
union ncr_t {
    struct {
        uint32_t init:1;	// 0
        uint32_t trie:1;	// 1
        uint32_t lecie:1;	// 2
        uint32_t alie:1;	// 3
        uint32_t candis:1;	// 4
        uint32_t :1;
        uint32_t cce:1;	// 6
        uint32_t calm:1;	// 7
        uint32_t susen:1;	// 8
    };
    uint32_t raw;
};
union necnt_t {
    struct {
        uint32_t rec:8;	// 0
        uint32_t tec:8;	// 8
        uint32_t ewrnlvl:8;	// 16
        uint32_t letd:1;	// 24
        uint32_t leinc:1;	// 25
    };
    uint32_t raw;
};
union nfcr_t {
    struct {
        uint32_t cfc:16;	// 0
        uint32_t cfsel:3;	// 16
        uint32_t cfmod:2;	// 19
        uint32_t :1;
        uint32_t cfcie:1;	// 22
        uint32_t cfcov:1;	// 23
    };
    uint32_t raw;
};
union nipr_t {
    struct {
        uint32_t alinp:3;	// 0
        uint32_t :1;
        uint32_t lecinp:3;	// 4
        uint32_t :1;
        uint32_t trinp:3;	// 8
        uint32_t :1;
        uint32_t cfcinp:3;	// 12
    };
    uint32_t raw;
};
union npcr_t {
    struct {
        uint32_t rxsel:3;	// 0
        uint32_t :5;
        uint32_t lbm:1;	// 8
    };
    uint32_t raw;
};
union nsr_t {
    struct {
        uint32_t lec:3;	// 0
        uint32_t txok:1;	// 3
        uint32_t rxok:1;	// 4
        uint32_t alert:1;	// 5
        uint32_t ewrn:1;	// 6
        uint32_t boff:1;	// 7
        uint32_t lle:1;	// 8
        uint32_t loe:1;	// 9
        uint32_t susack:1;	// 10
    };
    uint32_t raw;
};
}
namespace ccu4_cc4_ns {
union cmc_t {
    struct {
        uint32_t strts:2;	// 0
        uint32_t ends:2;	// 2
        uint32_t cap0s:2;	// 4
        uint32_t cap1s:2;	// 6
        uint32_t gates:2;	// 8
        uint32_t uds:2;	// 10
        uint32_t lds:2;	// 12
        uint32_t cnts:2;	// 14
        uint32_t ofs:1;	// 16
        uint32_t ts:1;	// 17
        uint32_t mos:2;	// 18
        uint32_t tce:1;	// 20
    };
    uint32_t raw;
};
union cr_t {
    struct {
        uint32_t cr:16;	// 0
    };
    uint32_t raw;
};
union crs_t {
    struct {
        uint32_t crs:16;	// 0
    };
    uint32_t raw;
};
union cv_t {
    struct {
        uint32_t captv:16;	// 0
        uint32_t fpcv:4;	// 16
        uint32_t ffl:1;	// 20
    };
    uint32_t raw;
};
union dit_t {
    struct {
        uint32_t dcv:4;	// 0
        uint32_t :4;
        uint32_t dcnt:4;	// 8
    };
    uint32_t raw;
};
union dits_t {
    struct {
        uint32_t dcvs:4;	// 0
    };
    uint32_t raw;
};
union fpc_t {
    struct {
        uint32_t pcmp:4;	// 0
        uint32_t :4;
        uint32_t pval:4;	// 8
    };
    uint32_t raw;
};
union fpcs_t {
    struct {
        uint32_t pcmp:4;	// 0
    };
    uint32_t raw;
};
union ins_t {
    struct {
        uint32_t ev0is:4;	// 0
        uint32_t ev1is:4;	// 4
        uint32_t ev2is:4;	// 8
        uint32_t :4;
        uint32_t ev0em:2;	// 16
        uint32_t ev1em:2;	// 18
        uint32_t ev2em:2;	// 20
        uint32_t ev0lm:1;	// 22
        uint32_t ev1lm:1;	// 23
        uint32_t ev2lm:1;	// 24
        uint32_t lpf0m:2;	// 25
        uint32_t lpf1m:2;	// 27
        uint32_t lpf2m:2;	// 29
    };
    uint32_t raw;
};
union inte_t {
    struct {
        uint32_t pme:1;	// 0
        uint32_t ome:1;	// 1
        uint32_t cmue:1;	// 2
        uint32_t cmde:1;	// 3
        uint32_t :4;
        uint32_t e0ae:1;	// 8
        uint32_t e1ae:1;	// 9
        uint32_t e2ae:1;	// 10
    };
    uint32_t raw;
};
union ints_t {
    struct {
        uint32_t pmus:1;	// 0
        uint32_t omds:1;	// 1
        uint32_t cmus:1;	// 2
        uint32_t cmds:1;	// 3
        uint32_t :4;
        uint32_t e0as:1;	// 8
        uint32_t e1as:1;	// 9
        uint32_t e2as:1;	// 10
        uint32_t trpf:1;	// 11
    };
    uint32_t raw;
};
union pr_t {
    struct {
        uint32_t pr:16;	// 0
    };
    uint32_t raw;
};
union prs_t {
    struct {
        uint32_t prs:16;	// 0
    };
    uint32_t raw;
};
union psc_t {
    struct {
        uint32_t psiv:4;	// 0
    };
    uint32_t raw;
};
union psl_t {
    struct {
        uint32_t psl:1;	// 0
    };
    uint32_t raw;
};
union srs_t {
    struct {
        uint32_t posr:2;	// 0
        uint32_t cmsr:2;	// 2
        uint32_t :4;
        uint32_t e0sr:2;	// 8
        uint32_t e1sr:2;	// 10
        uint32_t e2sr:2;	// 12
    };
    uint32_t raw;
};
union swr_t {
    struct {
        uint32_t rpm:1;	// 0
        uint32_t rom:1;	// 1
        uint32_t rcmu:1;	// 2
        uint32_t rcmd:1;	// 3
        uint32_t :4;
        uint32_t re0a:1;	// 8
        uint32_t re1a:1;	// 9
        uint32_t re2a:1;	// 10
        uint32_t rtrpf:1;	// 11
    };
    uint32_t raw;
};
union sws_t {
    struct {
        uint32_t spm:1;	// 0
        uint32_t som:1;	// 1
        uint32_t scmu:1;	// 2
        uint32_t scmd:1;	// 3
        uint32_t :4;
        uint32_t se0a:1;	// 8
        uint32_t se1a:1;	// 9
        uint32_t se2a:1;	// 10
        uint32_t strpf:1;	// 11
    };
    uint32_t raw;
};
union tc_t {
    struct {
        uint32_t tcm:1;	// 0
        uint32_t tssm:1;	// 1
        uint32_t clst:1;	// 2
        uint32_t cmod:1;	// 3
        uint32_t ecm:1;	// 4
        uint32_t capc:2;	// 5
        uint32_t :1;
        uint32_t endm:2;	// 8
        uint32_t strm:1;	// 10
        uint32_t sce:1;	// 11
        uint32_t ccs:1;	// 12
        uint32_t dithe:2;	// 13
        uint32_t dim:1;	// 15
        uint32_t fpe:1;	// 16
        uint32_t trape:1;	// 17
        uint32_t :3;
        uint32_t trpse:1;	// 21
        uint32_t trpsw:1;	// 22
        uint32_t ems:1;	// 23
        uint32_t emt:1;	// 24
        uint32_t mcme:1;	// 25
    };
    uint32_t raw;
};
union tcclr_t {
    struct {
        uint32_t trbc:1;	// 0
        uint32_t tcc:1;	// 1
        uint32_t ditc:1;	// 2
    };
    uint32_t raw;
};
union tcset_t {
    struct {
        uint32_t trbs:1;	// 0
    };
    uint32_t raw;
};
union tcst_t {
    struct {
        uint32_t trb:1;	// 0
        uint32_t cdir:1;	// 1
    };
    uint32_t raw;
};
union timer_t {
    struct {
        uint32_t tval:16;	// 0
    };
    uint32_t raw;
};
}
namespace ccu8_cc8_ns {
union chc_t {
    struct {
        uint32_t ase:1;	// 0
        uint32_t ocs1:1;	// 1
        uint32_t ocs2:1;	// 2
        uint32_t ocs3:1;	// 3
        uint32_t ocs4:1;	// 4
    };
    uint32_t raw;
};
union cmc_t {
    struct {
        uint32_t strts:2;	// 0
        uint32_t ends:2;	// 2
        uint32_t cap0s:2;	// 4
        uint32_t cap1s:2;	// 6
        uint32_t gates:2;	// 8
        uint32_t uds:2;	// 10
        uint32_t lds:2;	// 12
        uint32_t cnts:2;	// 14
        uint32_t ofs:1;	// 16
        uint32_t ts:1;	// 17
        uint32_t mos:2;	// 18
        uint32_t tce:1;	// 20
    };
    uint32_t raw;
};
union cr1_t {
    struct {
        uint32_t cr1:16;	// 0
    };
    uint32_t raw;
};
union cr1s_t {
    struct {
        uint32_t cr1s:16;	// 0
    };
    uint32_t raw;
};
union cr2_t {
    struct {
        uint32_t cr2:16;	// 0
    };
    uint32_t raw;
};
union cr2s_t {
    struct {
        uint32_t cr2s:16;	// 0
    };
    uint32_t raw;
};
union cv_t {
    struct {
        uint32_t captv:16;	// 0
        uint32_t fpcv:4;	// 16
        uint32_t ffl:1;	// 20
    };
    uint32_t raw;
};
union dc1r_t {
    struct {
        uint32_t dt1r:8;	// 0
        uint32_t dt1f:8;	// 8
    };
    uint32_t raw;
};
union dc2r_t {
    struct {
        uint32_t dt2r:8;	// 0
        uint32_t dt2f:8;	// 8
    };
    uint32_t raw;
};
union dit_t {
    struct {
        uint32_t dcv:4;	// 0
        uint32_t :4;
        uint32_t dcnt:4;	// 8
    };
    uint32_t raw;
};
union dits_t {
    struct {
        uint32_t dcvs:4;	// 0
    };
    uint32_t raw;
};
union dtc_t {
    struct {
        uint32_t dte1:1;	// 0
        uint32_t dte2:1;	// 1
        uint32_t dcen1:1;	// 2
        uint32_t dcen2:1;	// 3
        uint32_t dcen3:1;	// 4
        uint32_t dcen4:1;	// 5
        uint32_t dtcc:2;	// 6
    };
    uint32_t raw;
};
union fpc_t {
    struct {
        uint32_t pcmp:4;	// 0
        uint32_t :4;
        uint32_t pval:4;	// 8
    };
    uint32_t raw;
};
union fpcs_t {
    struct {
        uint32_t pcmp:4;	// 0
    };
    uint32_t raw;
};
union ins_t {
    struct {
        uint32_t ev0is:4;	// 0
        uint32_t ev1is:4;	// 4
        uint32_t ev2is:4;	// 8
        uint32_t :4;
        uint32_t ev0em:2;	// 16
        uint32_t ev1em:2;	// 18
        uint32_t ev2em:2;	// 20
        uint32_t ev0lm:1;	// 22
        uint32_t ev1lm:1;	// 23
        uint32_t ev2lm:1;	// 24
        uint32_t lpf0m:2;	// 25
        uint32_t lpf1m:2;	// 27
        uint32_t lpf2m:2;	// 29
    };
    uint32_t raw;
};
union inte_t {
    struct {
        uint32_t pme:1;	// 0
        uint32_t ome:1;	// 1
        uint32_t cmu1e:1;	// 2
        uint32_t cmd1e:1;	// 3
        uint32_t cmu2e:1;	// 4
        uint32_t cmd2e:1;	// 5
        uint32_t :2;
        uint32_t e0ae:1;	// 8
        uint32_t e1ae:1;	// 9
        uint32_t e2ae:1;	// 10
    };
    uint32_t raw;
};
union ints_t {
    struct {
        uint32_t pmus:1;	// 0
        uint32_t omds:1;	// 1
        uint32_t cmu1s:1;	// 2
        uint32_t cmd1s:1;	// 3
        uint32_t cmu2s:1;	// 4
        uint32_t cmd2s:1;	// 5
        uint32_t :2;
        uint32_t e0as:1;	// 8
        uint32_t e1as:1;	// 9
        uint32_t e2as:1;	// 10
        uint32_t trpf:1;	// 11
    };
    uint32_t raw;
};
union pr_t {
    struct {
        uint32_t pr:16;	// 0
    };
    uint32_t raw;
};
union prs_t {
    struct {
        uint32_t prs:16;	// 0
    };
    uint32_t raw;
};
union psc_t {
    struct {
        uint32_t psiv:4;	// 0
    };
    uint32_t raw;
};
union psl_t {
    struct {
        uint32_t psl11:1;	// 0
        uint32_t psl12:1;	// 1
        uint32_t psl21:1;	// 2
        uint32_t psl22:1;	// 3
    };
    uint32_t raw;
};
union srs_t {
    struct {
        uint32_t posr:2;	// 0
        uint32_t cm1sr:2;	// 2
        uint32_t cm2sr:2;	// 4
        uint32_t :2;
        uint32_t e0sr:2;	// 8
        uint32_t e1sr:2;	// 10
        uint32_t e2sr:2;	// 12
    };
    uint32_t raw;
};
union stc_t {
    struct {
        uint32_t cse:1;	// 0
        uint32_t stm:2;	// 1
    };
    uint32_t raw;
};
union swr_t {
    struct {
        uint32_t rpm:1;	// 0
        uint32_t rom:1;	// 1
        uint32_t rcm1u:1;	// 2
        uint32_t rcm1d:1;	// 3
        uint32_t rcm2u:1;	// 4
        uint32_t rcm2d:1;	// 5
        uint32_t :2;
        uint32_t re0a:1;	// 8
        uint32_t re1a:1;	// 9
        uint32_t re2a:1;	// 10
        uint32_t rtrpf:1;	// 11
    };
    uint32_t raw;
};
union sws_t {
    struct {
        uint32_t spm:1;	// 0
        uint32_t som:1;	// 1
        uint32_t scm1u:1;	// 2
        uint32_t scm1d:1;	// 3
        uint32_t scm2u:1;	// 4
        uint32_t scm2d:1;	// 5
        uint32_t :2;
        uint32_t se0a:1;	// 8
        uint32_t se1a:1;	// 9
        uint32_t se2a:1;	// 10
        uint32_t strpf:1;	// 11
    };
    uint32_t raw;
};
union tc_t {
    struct {
        uint32_t tcm:1;	// 0
        uint32_t tssm:1;	// 1
        uint32_t clst:1;	// 2
        uint32_t cmod:1;	// 3
        uint32_t ecm:1;	// 4
        uint32_t capc:2;	// 5
        uint32_t tls:1;	// 7
        uint32_t endm:2;	// 8
        uint32_t strm:1;	// 10
        uint32_t sce:1;	// 11
        uint32_t ccs:1;	// 12
        uint32_t dithe:2;	// 13
        uint32_t dim:1;	// 15
        uint32_t fpe:1;	// 16
        uint32_t trape0:1;	// 17
        uint32_t trape1:1;	// 18
        uint32_t trape2:1;	// 19
        uint32_t trape3:1;	// 20
        uint32_t trpse:1;	// 21
        uint32_t trpsw:1;	// 22
        uint32_t ems:1;	// 23
        uint32_t emt:1;	// 24
        uint32_t mcme1:1;	// 25
        uint32_t mcme2:1;	// 26
        uint32_t eme:2;	// 27
        uint32_t stos:2;	// 29
    };
    uint32_t raw;
};
union tcclr_t {
    struct {
        uint32_t trbc:1;	// 0
        uint32_t tcc:1;	// 1
        uint32_t ditc:1;	// 2
        uint32_t dtc1c:1;	// 3
        uint32_t dtc2c:1;	// 4
    };
    uint32_t raw;
};
union tcset_t {
    struct {
        uint32_t trbs:1;	// 0
    };
    uint32_t raw;
};
union tcst_t {
    struct {
        uint32_t trb:1;	// 0
        uint32_t cdir:1;	// 1
        uint32_t :1;
        uint32_t dtr1:1;	// 3
        uint32_t dtr2:1;	// 4
    };
    uint32_t raw;
};
union timer_t {
    struct {
        uint32_t tval:16;	// 0
    };
    uint32_t raw;
};
}
namespace dsd_ns {
union id_t {
    struct {
        uint32_t mod_rev:8;	// 0
        uint32_t mod_type:8;	// 8
        uint32_t mod_number:16;	// 16
    };
    uint32_t raw;
};
union ocs_t {
    struct {
        uint32_t :28;
        uint32_t sus_p:1;	// 28
    };
    uint32_t raw;
};
}
namespace dsd_ch_ns {
union boundsel_t {
    struct {
        uint32_t boundaryl:16;	// 0
        uint32_t boundaryu:16;	// 16
    };
    uint32_t raw;
};
union cgsync_t {
    struct {
        uint32_t sdcount:8;	// 0
        uint32_t sdcap:8;	// 8
        uint32_t sdpos:8;	// 16
        uint32_t sdneg:8;	// 24
    };
    uint32_t raw;
};
union dicfg_t {
    struct {
        uint32_t dsrc:4;	// 0
        uint32_t :3;
        uint32_t dswc:1;	// 7
        uint32_t itrmode:2;	// 8
        uint32_t tstrmode:2;	// 10
        uint32_t trsel:3;	// 12
        uint32_t trwc:1;	// 15
        uint32_t csrc:4;	// 16
        uint32_t strobe:4;	// 20
        uint32_t :7;
        uint32_t scwc:1;	// 31
    };
    uint32_t raw;
};
union fcfga_t {
    struct {
        uint32_t cfadf:8;	// 0
        uint32_t cfac:2;	// 8
        uint32_t srga:2;	// 10
        uint32_t esel:2;	// 12
        uint32_t egt:1;	// 14
        uint32_t :9;
        uint32_t cfadcnt:8;	// 24
    };
    uint32_t raw;
};
union fcfgc_t {
    struct {
        uint32_t cfmdf:8;	// 0
        uint32_t cfmc:2;	// 8
        uint32_t cfen:1;	// 10
        uint32_t :3;
        uint32_t srgm:2;	// 14
        uint32_t cfmsv:8;	// 16
        uint32_t cfmdcnt:8;	// 24
    };
    uint32_t raw;
};
union iwctr_t {
    struct {
        uint32_t nvalcnt:6;	// 0
        uint32_t :1;
        uint32_t inten:1;	// 7
        uint32_t repcnt:4;	// 8
        uint32_t repval:4;	// 12
        uint32_t nvaldis:6;	// 16
        uint32_t :1;
        uint32_t iws:1;	// 23
        uint32_t nvalint:6;	// 24
    };
    uint32_t raw;
};
union modcfg_t {
    struct {
        uint32_t :16;
        uint32_t divm:4;	// 16
        uint32_t :3;
        uint32_t dwc:1;	// 23
    };
    uint32_t raw;
};
union offm_t {
    struct {
        uint32_t offset:16;	// 0
    };
    uint32_t raw;
};
union rectcfg_t {
    struct {
        uint32_t rfen:1;	// 0
        uint32_t :3;
        uint32_t ssrc:2;	// 4
        uint32_t :9;
        uint32_t sdval:1;	// 15
        uint32_t :14;
        uint32_t sgncs:1;	// 30
        uint32_t sgnd:1;	// 31
    };
    uint32_t raw;
};
union resa_t {
    struct {
        uint32_t result:16;	// 0
    };
    uint32_t raw;
};
union resm_t {
    struct {
        uint32_t result:16;	// 0
    };
    uint32_t raw;
};
union tstmp_t {
    struct {
        uint32_t result:16;	// 0
        uint32_t cfmdcnt:8;	// 16
        uint32_t nvalcnt:6;	// 24
    };
    uint32_t raw;
};
}
namespace eth_ns {
union ahb_status_t {
    struct {
        uint32_t ahbms:1;	// 0
    };
    uint32_t raw;
};
union bus_mode_t {
    struct {
        uint32_t swr:1;	// 0
        uint32_t da:1;	// 1
        uint32_t dsl:5;	// 2
        uint32_t atds:1;	// 7
        uint32_t pbl:6;	// 8
        uint32_t pr:2;	// 14
        uint32_t fb:1;	// 16
        uint32_t rpbl:6;	// 17
        uint32_t usp:1;	// 23
        uint32_t eight:1;	// 24
        uint32_t aal:1;	// 25
        uint32_t mb:1;	// 26
        uint32_t txpr:1;	// 27
        uint32_t prwg:2;	// 28
    };
    uint32_t raw;
};
union current_host_receive_buffer_address_t {
    struct {
        uint32_t currbufaptr:32;	// 0
    };
    uint32_t raw;
};
union current_host_receive_descriptor_t {
    struct {
        uint32_t currdesaptr:32;	// 0
    };
    uint32_t raw;
};
union current_host_transmit_buffer_address_t {
    struct {
        uint32_t curtbufaptr:32;	// 0
    };
    uint32_t raw;
};
union current_host_transmit_descriptor_t {
    struct {
        uint32_t curtdesaptr:32;	// 0
    };
    uint32_t raw;
};
union flow_control_t {
    struct {
        uint32_t fca_bpa:1;	// 0
        uint32_t tfe:1;	// 1
        uint32_t rfe:1;	// 2
        uint32_t up:1;	// 3
        uint32_t plt:2;	// 4
        uint32_t :1;
        uint32_t dzpq:1;	// 7
        uint32_t :8;
        uint32_t pt:16;	// 16
    };
    uint32_t raw;
};
union gmii_address_t {
    struct {
        uint32_t mb:1;	// 0
        uint32_t mw:1;	// 1
        uint32_t cr:4;	// 2
        uint32_t mr:5;	// 6
        uint32_t pa:5;	// 11
    };
    uint32_t raw;
};
union gmii_data_t {
    struct {
        uint32_t md:16;	// 0
    };
    uint32_t raw;
};
union hash_table_high_t {
    struct {
        uint32_t hth:32;	// 0
    };
    uint32_t raw;
};
union hash_table_low_t {
    struct {
        uint32_t htl:32;	// 0
    };
    uint32_t raw;
};
union hw_feature_t {
    struct {
        uint32_t miisel:1;	// 0
        uint32_t gmiisel:1;	// 1
        uint32_t hdsel:1;	// 2
        uint32_t exthashen:1;	// 3
        uint32_t hashsel:1;	// 4
        uint32_t addmacadrsel:1;	// 5
        uint32_t pcssel:1;	// 6
        uint32_t l3l4fltren:1;	// 7
        uint32_t smasel:1;	// 8
        uint32_t rwksel:1;	// 9
        uint32_t mgksel:1;	// 10
        uint32_t mmcsel:1;	// 11
        uint32_t tsver1sel:1;	// 12
        uint32_t tsver2sel:1;	// 13
        uint32_t eeesel:1;	// 14
        uint32_t avsel:1;	// 15
        uint32_t txcoesel:1;	// 16
        uint32_t rxtyp1coe:1;	// 17
        uint32_t rxtyp2coe:1;	// 18
        uint32_t rxfifosize:1;	// 19
        uint32_t rxchcnt:2;	// 20
        uint32_t txchcnt:2;	// 22
        uint32_t enhdessel:1;	// 24
        uint32_t inttsen:1;	// 25
        uint32_t flexippsen:1;	// 26
        uint32_t savlanins:1;	// 27
        uint32_t actphyif:3;	// 28
    };
    uint32_t raw;
};
union interrupt_enable_t {
    struct {
        uint32_t tie:1;	// 0
        uint32_t tse:1;	// 1
        uint32_t tue:1;	// 2
        uint32_t tje:1;	// 3
        uint32_t ove:1;	// 4
        uint32_t une:1;	// 5
        uint32_t rie:1;	// 6
        uint32_t rue:1;	// 7
        uint32_t rse:1;	// 8
        uint32_t rwe:1;	// 9
        uint32_t ete:1;	// 10
        uint32_t :2;
        uint32_t fbe:1;	// 13
        uint32_t ere:1;	// 14
        uint32_t aie:1;	// 15
        uint32_t nie:1;	// 16
    };
    uint32_t raw;
};
union interrupt_mask_t {
    struct {
        uint32_t :3;
        uint32_t pmtim:1;	// 3
        uint32_t :5;
        uint32_t tsim:1;	// 9
    };
    uint32_t raw;
};
union interrupt_status_t {
    struct {
        uint32_t :3;
        uint32_t pmtis:1;	// 3
        uint32_t mmcis:1;	// 4
        uint32_t mmcrxis:1;	// 5
        uint32_t mmctxis:1;	// 6
        uint32_t mmcrxipis:1;	// 7
        uint32_t :1;
        uint32_t tsis:1;	// 9
    };
    uint32_t raw;
};
union mac_address0_high_t {
    struct {
        uint32_t addrhi:16;	// 0
        uint32_t :15;
        uint32_t ae:1;	// 31
    };
    uint32_t raw;
};
union mac_address0_low_t {
    struct {
        uint32_t addrlo:32;	// 0
    };
    uint32_t raw;
};
union mac_address1_high_t {
    struct {
        uint32_t addrhi:16;	// 0
        uint32_t :8;
        uint32_t mbc:6;	// 24
        uint32_t sa:1;	// 30
        uint32_t ae:1;	// 31
    };
    uint32_t raw;
};
union mac_address1_low_t {
    struct {
        uint32_t addrlo:32;	// 0
    };
    uint32_t raw;
};
union mac_address2_high_t {
    struct {
        uint32_t addrhi:16;	// 0
        uint32_t :8;
        uint32_t mbc:6;	// 24
        uint32_t sa:1;	// 30
        uint32_t ae:1;	// 31
    };
    uint32_t raw;
};
union mac_address2_low_t {
    struct {
        uint32_t addrlo:32;	// 0
    };
    uint32_t raw;
};
union mac_address3_high_t {
    struct {
        uint32_t addrhi:16;	// 0
        uint32_t :8;
        uint32_t mbc:6;	// 24
        uint32_t sa:1;	// 30
        uint32_t ae:1;	// 31
    };
    uint32_t raw;
};
union mac_address3_low_t {
    struct {
        uint32_t addrlo:32;	// 0
    };
    uint32_t raw;
};
union mac_configuration_t {
    struct {
        uint32_t prelen:2;	// 0
        uint32_t re:1;	// 2
        uint32_t te:1;	// 3
        uint32_t dc:1;	// 4
        uint32_t bl:2;	// 5
        uint32_t acs:1;	// 7
        uint32_t :1;
        uint32_t dr:1;	// 9
        uint32_t ipc:1;	// 10
        uint32_t dm:1;	// 11
        uint32_t lm:1;	// 12
        uint32_t do_:1;	// 13
        uint32_t fes:1;	// 14
        uint32_t :1;
        uint32_t dcrs:1;	// 16
        uint32_t ifg:3;	// 17
        uint32_t je:1;	// 20
        uint32_t be:1;	// 21
        uint32_t jd:1;	// 22
        uint32_t wd:1;	// 23
        uint32_t tc:1;	// 24
        uint32_t cst:1;	// 25
        uint32_t :1;
        uint32_t twokpe:1;	// 27
        uint32_t sarc:3;	// 28
    };
    uint32_t raw;
};
union mac_frame_filter_t {
    struct {
        uint32_t pr:1;	// 0
        uint32_t huc:1;	// 1
        uint32_t hmc:1;	// 2
        uint32_t daif:1;	// 3
        uint32_t pm:1;	// 4
        uint32_t dbf:1;	// 5
        uint32_t pcf:2;	// 6
        uint32_t saif:1;	// 8
        uint32_t saf:1;	// 9
        uint32_t hpf:1;	// 10
        uint32_t :5;
        uint32_t vtfe:1;	// 16
        uint32_t :3;
        uint32_t ipfe:1;	// 20
        uint32_t dntu:1;	// 21
        uint32_t :9;
        uint32_t ra:1;	// 31
    };
    uint32_t raw;
};
union missed_frame_and_buffer_overflow_counter_t {
    struct {
        uint32_t misfrmcnt:16;	// 0
        uint32_t miscntovf:1;	// 16
        uint32_t ovffrmcnt:11;	// 17
        uint32_t ovfcntovf:1;	// 28
    };
    uint32_t raw;
};
union mmc_control_t {
    struct {
        uint32_t cntrst:1;	// 0
        uint32_t cntstopro:1;	// 1
        uint32_t rstonrd:1;	// 2
        uint32_t cntfreez:1;	// 3
        uint32_t cntprst:1;	// 4
        uint32_t cntprstlvl:1;	// 5
        uint32_t :2;
        uint32_t ucdbc:1;	// 8
    };
    uint32_t raw;
};
union mmc_ipc_receive_interrupt_t {
    struct {
        uint32_t rxipv4gfis:1;	// 0
        uint32_t rxipv4herfis:1;	// 1
        uint32_t rxipv4nopayfis:1;	// 2
        uint32_t rxipv4fragfis:1;	// 3
        uint32_t rxipv4udsblfis:1;	// 4
        uint32_t rxipv6gfis:1;	// 5
        uint32_t rxipv6herfis:1;	// 6
        uint32_t rxipv6nopayfis:1;	// 7
        uint32_t rxudpgfis:1;	// 8
        uint32_t rxudperfis:1;	// 9
        uint32_t rxtcpgfis:1;	// 10
        uint32_t rxtcperfis:1;	// 11
        uint32_t rxicmpgfis:1;	// 12
        uint32_t rxicmperfis:1;	// 13
        uint32_t :2;
        uint32_t rxipv4gois:1;	// 16
        uint32_t rxipv4herois:1;	// 17
        uint32_t rxipv4nopayois:1;	// 18
        uint32_t rxipv4fragois:1;	// 19
        uint32_t rxipv4udsblois:1;	// 20
        uint32_t rxipv6gois:1;	// 21
        uint32_t rxipv6herois:1;	// 22
        uint32_t rxipv6nopayois:1;	// 23
        uint32_t rxudpgois:1;	// 24
        uint32_t rxudperois:1;	// 25
        uint32_t rxtcpgois:1;	// 26
        uint32_t rxtcperois:1;	// 27
        uint32_t rxicmpgois:1;	// 28
        uint32_t rxicmperois:1;	// 29
    };
    uint32_t raw;
};
union mmc_ipc_receive_interrupt_mask_t {
    struct {
        uint32_t rxipv4gfim:1;	// 0
        uint32_t rxipv4herfim:1;	// 1
        uint32_t rxipv4nopayfim:1;	// 2
        uint32_t rxipv4fragfim:1;	// 3
        uint32_t rxipv4udsblfim:1;	// 4
        uint32_t rxipv6gfim:1;	// 5
        uint32_t rxipv6herfim:1;	// 6
        uint32_t rxipv6nopayfim:1;	// 7
        uint32_t rxudpgfim:1;	// 8
        uint32_t rxudperfim:1;	// 9
        uint32_t rxtcpgfim:1;	// 10
        uint32_t rxtcperfim:1;	// 11
        uint32_t rxicmpgfim:1;	// 12
        uint32_t rxicmperfim:1;	// 13
        uint32_t :2;
        uint32_t rxipv4goim:1;	// 16
        uint32_t rxipv4heroim:1;	// 17
        uint32_t rxipv4nopayoim:1;	// 18
        uint32_t rxipv4fragoim:1;	// 19
        uint32_t rxipv4udsbloim:1;	// 20
        uint32_t rxipv6goim:1;	// 21
        uint32_t rxipv6heroim:1;	// 22
        uint32_t rxipv6nopayoim:1;	// 23
        uint32_t rxudpgoim:1;	// 24
        uint32_t rxudperoim:1;	// 25
        uint32_t rxtcpgoim:1;	// 26
        uint32_t rxtcperoim:1;	// 27
        uint32_t rxicmpgoim:1;	// 28
        uint32_t rxicmperoim:1;	// 29
    };
    uint32_t raw;
};
union mmc_receive_interrupt_t {
    struct {
        uint32_t rxgbfrmis:1;	// 0
        uint32_t rxgboctis:1;	// 1
        uint32_t rxgoctis:1;	// 2
        uint32_t rxbcgfis:1;	// 3
        uint32_t rxmcgfis:1;	// 4
        uint32_t rxcrcerfis:1;	// 5
        uint32_t rxalgnerfis:1;	// 6
        uint32_t rxruntfis:1;	// 7
        uint32_t rxjaberfis:1;	// 8
        uint32_t rxusizegfis:1;	// 9
        uint32_t rxosizegfis:1;	// 10
        uint32_t rx64octgbfis:1;	// 11
        uint32_t rx65t127octgbfis:1;	// 12
        uint32_t rx128t255octgbfis:1;	// 13
        uint32_t rx256t511octgbfis:1;	// 14
        uint32_t rx512t1023octgbfis:1;	// 15
        uint32_t rx1024tmaxoctgbfis:1;	// 16
        uint32_t rxucgfis:1;	// 17
        uint32_t rxlenerfis:1;	// 18
        uint32_t rxorangefis:1;	// 19
        uint32_t rxpausfis:1;	// 20
        uint32_t rxfovfis:1;	// 21
        uint32_t rxvlangbfis:1;	// 22
        uint32_t rxwdogfis:1;	// 23
        uint32_t rxrcverrfis:1;	// 24
        uint32_t rxctrlfis:1;	// 25
    };
    uint32_t raw;
};
union mmc_receive_interrupt_mask_t {
    struct {
        uint32_t rxgbfrmim:1;	// 0
        uint32_t rxgboctim:1;	// 1
        uint32_t rxgoctim:1;	// 2
        uint32_t rxbcgfim:1;	// 3
        uint32_t rxmcgfim:1;	// 4
        uint32_t rxcrcerfim:1;	// 5
        uint32_t rxalgnerfim:1;	// 6
        uint32_t rxruntfim:1;	// 7
        uint32_t rxjaberfim:1;	// 8
        uint32_t rxusizegfim:1;	// 9
        uint32_t rxosizegfim:1;	// 10
        uint32_t rx64octgbfim:1;	// 11
        uint32_t rx65t127octgbfim:1;	// 12
        uint32_t rx128t255octgbfim:1;	// 13
        uint32_t rx256t511octgbfim:1;	// 14
        uint32_t rx512t1023octgbfim:1;	// 15
        uint32_t rx1024tmaxoctgbfim:1;	// 16
        uint32_t rxucgfim:1;	// 17
        uint32_t rxlenerfim:1;	// 18
        uint32_t rxorangefim:1;	// 19
        uint32_t rxpausfim:1;	// 20
        uint32_t rxfovfim:1;	// 21
        uint32_t rxvlangbfim:1;	// 22
        uint32_t rxwdogfim:1;	// 23
        uint32_t rxrcverrfim:1;	// 24
        uint32_t rxctrlfim:1;	// 25
    };
    uint32_t raw;
};
union mmc_transmit_interrupt_t {
    struct {
        uint32_t txgboctis:1;	// 0
        uint32_t txgbfrmis:1;	// 1
        uint32_t txbcgfis:1;	// 2
        uint32_t txmcgfis:1;	// 3
        uint32_t tx64octgbfis:1;	// 4
        uint32_t tx65t127octgbfis:1;	// 5
        uint32_t tx128t255octgbfis:1;	// 6
        uint32_t tx256t511octgbfis:1;	// 7
        uint32_t tx512t1023octgbfis:1;	// 8
        uint32_t tx1024tmaxoctgbfis:1;	// 9
        uint32_t txucgbfis:1;	// 10
        uint32_t txmcgbfis:1;	// 11
        uint32_t txbcgbfis:1;	// 12
        uint32_t txuflowerfis:1;	// 13
        uint32_t txscolgfis:1;	// 14
        uint32_t txmcolgfis:1;	// 15
        uint32_t txdeffis:1;	// 16
        uint32_t txlatcolfis:1;	// 17
        uint32_t txexcolfis:1;	// 18
        uint32_t txcarerfis:1;	// 19
        uint32_t txgoctis:1;	// 20
        uint32_t txgfrmis:1;	// 21
        uint32_t txexdeffis:1;	// 22
        uint32_t txpausfis:1;	// 23
        uint32_t txvlangfis:1;	// 24
        uint32_t txosizegfis:1;	// 25
    };
    uint32_t raw;
};
union mmc_transmit_interrupt_mask_t {
    struct {
        uint32_t txgboctim:1;	// 0
        uint32_t txgbfrmim:1;	// 1
        uint32_t txbcgfim:1;	// 2
        uint32_t txmcgfim:1;	// 3
        uint32_t tx64octgbfim:1;	// 4
        uint32_t tx65t127octgbfim:1;	// 5
        uint32_t tx128t255octgbfim:1;	// 6
        uint32_t tx256t511octgbfim:1;	// 7
        uint32_t tx512t1023octgbfim:1;	// 8
        uint32_t tx1024tmaxoctgbfim:1;	// 9
        uint32_t txucgbfim:1;	// 10
        uint32_t txmcgbfim:1;	// 11
        uint32_t txbcgbfim:1;	// 12
        uint32_t txuflowerfim:1;	// 13
        uint32_t txscolgfim:1;	// 14
        uint32_t txmcolgfim:1;	// 15
        uint32_t txdeffim:1;	// 16
        uint32_t txlatcolfim:1;	// 17
        uint32_t txexcolfim:1;	// 18
        uint32_t txcarerfim:1;	// 19
        uint32_t txgoctim:1;	// 20
        uint32_t txgfrmim:1;	// 21
        uint32_t txexdeffim:1;	// 22
        uint32_t txpausfim:1;	// 23
        uint32_t txvlangfim:1;	// 24
        uint32_t txosizegfim:1;	// 25
    };
    uint32_t raw;
};
union operation_mode_t {
    struct {
        uint32_t :1;
        uint32_t sr:1;	// 1
        uint32_t osf:1;	// 2
        uint32_t rtc:2;	// 3
        uint32_t :1;
        uint32_t fuf:1;	// 6
        uint32_t fef:1;	// 7
        uint32_t :5;
        uint32_t st:1;	// 13
        uint32_t ttc:3;	// 14
        uint32_t :3;
        uint32_t ftf:1;	// 20
        uint32_t tsf:1;	// 21
        uint32_t :2;
        uint32_t dff:1;	// 24
        uint32_t rsf:1;	// 25
        uint32_t dt:1;	// 26
    };
    uint32_t raw;
};
union pmt_control_status_t {
    struct {
        uint32_t pwrdwn:1;	// 0
        uint32_t mgkpkten:1;	// 1
        uint32_t rwkpkten:1;	// 2
        uint32_t :2;
        uint32_t mgkprcvd:1;	// 5
        uint32_t rwkprcvd:1;	// 6
        uint32_t :2;
        uint32_t glblucast:1;	// 9
        uint32_t :21;
        uint32_t rwkfiltrst:1;	// 31
    };
    uint32_t raw;
};
union pps_control_t {
    struct {
        uint32_t ppsctrl_ppscmd:4;	// 0
        uint32_t ppsen0:1;	// 4
        uint32_t trgtmodsel0:2;	// 5
        uint32_t :1;
        uint32_t ppscmd1:3;	// 8
        uint32_t :2;
        uint32_t trgtmodsel1:2;	// 13
        uint32_t :1;
        uint32_t ppscmd2:3;	// 16
        uint32_t :2;
        uint32_t trgtmodsel2:2;	// 21
        uint32_t :1;
        uint32_t ppscmd3:3;	// 24
        uint32_t :2;
        uint32_t trgtmodsel3:2;	// 29
    };
    uint32_t raw;
};
union receive_descriptor_list_address_t {
    struct {
        uint32_t :2;
        uint32_t rdesla_32:30;	// 2
    };
    uint32_t raw;
};
union receive_interrupt_watchdog_timer_t {
    struct {
        uint32_t riwt:8;	// 0
    };
    uint32_t raw;
};
union receive_poll_demand_t {
    struct {
        uint32_t rpd:32;	// 0
    };
    uint32_t raw;
};
union remote_wake_up_frame_filter_t {
    struct {
        uint32_t wkupfrmftr:32;	// 0
    };
    uint32_t raw;
};
union rxicmp_error_frames_t {
    struct {
        uint32_t rxicmperrfrm:32;	// 0
    };
    uint32_t raw;
};
union rxicmp_error_octets_t {
    struct {
        uint32_t rxicmperroct:32;	// 0
    };
    uint32_t raw;
};
union rxicmp_good_frames_t {
    struct {
        uint32_t rxicmpgdfrm:32;	// 0
    };
    uint32_t raw;
};
union rxicmp_good_octets_t {
    struct {
        uint32_t rxicmpgdoct:32;	// 0
    };
    uint32_t raw;
};
union rxipv4_fragmented_frames_t {
    struct {
        uint32_t rxipv4fragfrm:32;	// 0
    };
    uint32_t raw;
};
union rxipv4_fragmented_octets_t {
    struct {
        uint32_t rxipv4fragoct:32;	// 0
    };
    uint32_t raw;
};
union rxipv4_good_frames_t {
    struct {
        uint32_t rxipv4gdfrm:32;	// 0
    };
    uint32_t raw;
};
union rxipv4_good_octets_t {
    struct {
        uint32_t rxipv4gdoct:32;	// 0
    };
    uint32_t raw;
};
union rxipv4_header_error_frames_t {
    struct {
        uint32_t rxipv4hdrerrfrm:32;	// 0
    };
    uint32_t raw;
};
union rxipv4_header_error_octets_t {
    struct {
        uint32_t rxipv4hdrerroct:32;	// 0
    };
    uint32_t raw;
};
union rxipv4_no_payload_frames_t {
    struct {
        uint32_t rxipv4nopayfrm:32;	// 0
    };
    uint32_t raw;
};
union rxipv4_no_payload_octets_t {
    struct {
        uint32_t rxipv4nopayoct:32;	// 0
    };
    uint32_t raw;
};
union rxipv4_udp_checksum_disabled_frames_t {
    struct {
        uint32_t rxipv4udsblfrm:32;	// 0
    };
    uint32_t raw;
};
union rxipv4_udp_checksum_disable_octets_t {
    struct {
        uint32_t rxipv4udsbloct:32;	// 0
    };
    uint32_t raw;
};
union rxipv6_good_frames_t {
    struct {
        uint32_t rxipv6gdfrm:32;	// 0
    };
    uint32_t raw;
};
union rxipv6_good_octets_t {
    struct {
        uint32_t rxipv6gdoct:32;	// 0
    };
    uint32_t raw;
};
union rxipv6_header_error_frames_t {
    struct {
        uint32_t rxipv6hdrerrfrm:32;	// 0
    };
    uint32_t raw;
};
union rxipv6_header_error_octets_t {
    struct {
        uint32_t rxipv6hdrerroct:32;	// 0
    };
    uint32_t raw;
};
union rxipv6_no_payload_frames_t {
    struct {
        uint32_t rxipv6nopayfrm:32;	// 0
    };
    uint32_t raw;
};
union rxipv6_no_payload_octets_t {
    struct {
        uint32_t rxipv6nopayoct:32;	// 0
    };
    uint32_t raw;
};
union rxtcp_error_frames_t {
    struct {
        uint32_t rxtcperrfrm:32;	// 0
    };
    uint32_t raw;
};
union rxtcp_error_octets_t {
    struct {
        uint32_t rxtcperroct:32;	// 0
    };
    uint32_t raw;
};
union rxtcp_good_frames_t {
    struct {
        uint32_t rxtcpgdfrm:32;	// 0
    };
    uint32_t raw;
};
union rxtcp_good_octets_t {
    struct {
        uint32_t rxtcpgdoct:32;	// 0
    };
    uint32_t raw;
};
union rxudp_error_frames_t {
    struct {
        uint32_t rxudperrfrm:32;	// 0
    };
    uint32_t raw;
};
union rxudp_error_octets_t {
    struct {
        uint32_t rxudperroct:32;	// 0
    };
    uint32_t raw;
};
union rxudp_good_frames_t {
    struct {
        uint32_t rxudpgdfrm:32;	// 0
    };
    uint32_t raw;
};
union rxudp_good_octets_t {
    struct {
        uint32_t rxudpgdoct:32;	// 0
    };
    uint32_t raw;
};
union rx_1024tomaxoctets_frames_good_bad_t {
    struct {
        uint32_t rx1024_maxoctgb:32;	// 0
    };
    uint32_t raw;
};
union rx_128to255octets_frames_good_bad_t {
    struct {
        uint32_t rx128_255octgb:32;	// 0
    };
    uint32_t raw;
};
union rx_256to511octets_frames_good_bad_t {
    struct {
        uint32_t rx256_511octgb:32;	// 0
    };
    uint32_t raw;
};
union rx_512to1023octets_frames_good_bad_t {
    struct {
        uint32_t rx512_1023octgb:32;	// 0
    };
    uint32_t raw;
};
union rx_64octets_frames_good_bad_t {
    struct {
        uint32_t rx64octgb:32;	// 0
    };
    uint32_t raw;
};
union rx_65to127octets_frames_good_bad_t {
    struct {
        uint32_t rx65_127octgb:32;	// 0
    };
    uint32_t raw;
};
union rx_alignment_error_frames_t {
    struct {
        uint32_t rxalgnerr:32;	// 0
    };
    uint32_t raw;
};
union rx_broadcast_frames_good_t {
    struct {
        uint32_t rxbcastg:32;	// 0
    };
    uint32_t raw;
};
union rx_control_frames_good_t {
    struct {
        uint32_t rxctrlg:32;	// 0
    };
    uint32_t raw;
};
union rx_crc_error_frames_t {
    struct {
        uint32_t rxcrcerr:32;	// 0
    };
    uint32_t raw;
};
union rx_fifo_overflow_frames_t {
    struct {
        uint32_t rxfifoovfl:32;	// 0
    };
    uint32_t raw;
};
union rx_frames_count_good_bad_t {
    struct {
        uint32_t rxfrmgb:32;	// 0
    };
    uint32_t raw;
};
union rx_jabber_error_frames_t {
    struct {
        uint32_t rxjaberr:32;	// 0
    };
    uint32_t raw;
};
union rx_length_error_frames_t {
    struct {
        uint32_t rxlenerr:32;	// 0
    };
    uint32_t raw;
};
union rx_multicast_frames_good_t {
    struct {
        uint32_t rxmcastg:32;	// 0
    };
    uint32_t raw;
};
union rx_octet_count_good_t {
    struct {
        uint32_t rxoctg:32;	// 0
    };
    uint32_t raw;
};
union rx_octet_count_good_bad_t {
    struct {
        uint32_t rxoctgb:32;	// 0
    };
    uint32_t raw;
};
union rx_out_of_range_type_frames_t {
    struct {
        uint32_t rxoutofrng:32;	// 0
    };
    uint32_t raw;
};
union rx_oversize_frames_good_t {
    struct {
        uint32_t rxoverszg:32;	// 0
    };
    uint32_t raw;
};
union rx_pause_frames_t {
    struct {
        uint32_t rxpausefrm:32;	// 0
    };
    uint32_t raw;
};
union rx_receive_error_frames_t {
    struct {
        uint32_t rxrcverr:32;	// 0
    };
    uint32_t raw;
};
union rx_runt_error_frames_t {
    struct {
        uint32_t rxrunterr:32;	// 0
    };
    uint32_t raw;
};
union rx_undersize_frames_good_t {
    struct {
        uint32_t rxunderszg:32;	// 0
    };
    uint32_t raw;
};
union rx_unicast_frames_good_t {
    struct {
        uint32_t rxucastg:32;	// 0
    };
    uint32_t raw;
};
union rx_vlan_frames_good_bad_t {
    struct {
        uint32_t rxvlanfrgb:32;	// 0
    };
    uint32_t raw;
};
union rx_watchdog_error_frames_t {
    struct {
        uint32_t rxwdgerr:32;	// 0
    };
    uint32_t raw;
};
union sub_second_increment_t {
    struct {
        uint32_t ssinc:8;	// 0
    };
    uint32_t raw;
};
union system_time_higher_word_seconds_t {
    struct {
        uint32_t tshwr:16;	// 0
    };
    uint32_t raw;
};
union system_time_nanoseconds_t {
    struct {
        uint32_t tsss:31;	// 0
    };
    uint32_t raw;
};
union system_time_nanoseconds_update_t {
    struct {
        uint32_t tsss:31;	// 0
        uint32_t addsub:1;	// 31
    };
    uint32_t raw;
};
union system_time_seconds_t {
    struct {
        uint32_t tss:32;	// 0
    };
    uint32_t raw;
};
union system_time_seconds_update_t {
    struct {
        uint32_t tss:32;	// 0
    };
    uint32_t raw;
};
union target_time_nanoseconds_t {
    struct {
        uint32_t ttslo:31;	// 0
        uint32_t trgtbusy:1;	// 31
    };
    uint32_t raw;
};
union target_time_seconds_t {
    struct {
        uint32_t tstr:32;	// 0
    };
    uint32_t raw;
};
union timestamp_addend_t {
    struct {
        uint32_t tsar:32;	// 0
    };
    uint32_t raw;
};
union timestamp_control_t {
    struct {
        uint32_t tsena:1;	// 0
        uint32_t tscfupdt:1;	// 1
        uint32_t tsinit:1;	// 2
        uint32_t tsupdt:1;	// 3
        uint32_t tstrig:1;	// 4
        uint32_t tsaddreg:1;	// 5
        uint32_t :2;
        uint32_t tsenall:1;	// 8
        uint32_t tsctrlssr:1;	// 9
        uint32_t tsver2ena:1;	// 10
        uint32_t tsipena:1;	// 11
        uint32_t tsipv6ena:1;	// 12
        uint32_t tsipv4ena:1;	// 13
        uint32_t tsevntena:1;	// 14
        uint32_t tsmstrena:1;	// 15
        uint32_t snaptypsel:2;	// 16
        uint32_t tsenmacaddr:1;	// 18
    };
    uint32_t raw;
};
union timestamp_status_t {
    struct {
        uint32_t tssovf:1;	// 0
        uint32_t tstargt:1;	// 1
        uint32_t :1;
        uint32_t tstrgterr:1;	// 3
        uint32_t tstargt1:1;	// 4
        uint32_t tstrgterr1:1;	// 5
        uint32_t tstargt2:1;	// 6
        uint32_t tstrgterr2:1;	// 7
        uint32_t tstargt3:1;	// 8
        uint32_t tstrgterr3:1;	// 9
    };
    uint32_t raw;
};
union transmit_descriptor_list_address_t {
    struct {
        uint32_t :2;
        uint32_t tdesla_32:30;	// 2
    };
    uint32_t raw;
};
union transmit_poll_demand_t {
    struct {
        uint32_t tpd:32;	// 0
    };
    uint32_t raw;
};
union tx_1024tomaxoctets_frames_good_bad_t {
    struct {
        uint32_t tx1024_maxoctgb:32;	// 0
    };
    uint32_t raw;
};
union tx_128to255octets_frames_good_bad_t {
    struct {
        uint32_t tx128_255octgb:32;	// 0
    };
    uint32_t raw;
};
union tx_256to511octets_frames_good_bad_t {
    struct {
        uint32_t tx256_511octgb:32;	// 0
    };
    uint32_t raw;
};
union tx_512to1023octets_frames_good_bad_t {
    struct {
        uint32_t tx512_1023octgb:32;	// 0
    };
    uint32_t raw;
};
union tx_64octets_frames_good_bad_t {
    struct {
        uint32_t tx64octgb:32;	// 0
    };
    uint32_t raw;
};
union tx_65to127octets_frames_good_bad_t {
    struct {
        uint32_t tx65_127octgb:32;	// 0
    };
    uint32_t raw;
};
union tx_broadcast_frames_good_t {
    struct {
        uint32_t txbcastg:32;	// 0
    };
    uint32_t raw;
};
union tx_broadcast_frames_good_bad_t {
    struct {
        uint32_t txbcastgb:32;	// 0
    };
    uint32_t raw;
};
union tx_carrier_error_frames_t {
    struct {
        uint32_t txcarr:32;	// 0
    };
    uint32_t raw;
};
union tx_deferred_frames_t {
    struct {
        uint32_t txdefrd:32;	// 0
    };
    uint32_t raw;
};
union tx_excessive_collision_frames_t {
    struct {
        uint32_t txexscol:32;	// 0
    };
    uint32_t raw;
};
union tx_excessive_deferral_error_t {
    struct {
        uint32_t txexsdef:32;	// 0
    };
    uint32_t raw;
};
union tx_frame_count_good_t {
    struct {
        uint32_t txfrmg:32;	// 0
    };
    uint32_t raw;
};
union tx_frame_count_good_bad_t {
    struct {
        uint32_t txfrmgb:32;	// 0
    };
    uint32_t raw;
};
union tx_late_collision_frames_t {
    struct {
        uint32_t txlatecol:32;	// 0
    };
    uint32_t raw;
};
union tx_multicast_frames_good_t {
    struct {
        uint32_t txmcastg:32;	// 0
    };
    uint32_t raw;
};
union tx_multicast_frames_good_bad_t {
    struct {
        uint32_t txmcastgb:32;	// 0
    };
    uint32_t raw;
};
union tx_multiple_collision_good_frames_t {
    struct {
        uint32_t txmultcolg:32;	// 0
    };
    uint32_t raw;
};
union tx_octet_count_good_t {
    struct {
        uint32_t txoctg:32;	// 0
    };
    uint32_t raw;
};
union tx_octet_count_good_bad_t {
    struct {
        uint32_t txoctgb:32;	// 0
    };
    uint32_t raw;
};
union tx_osize_frames_good_t {
    struct {
        uint32_t txosizg:32;	// 0
    };
    uint32_t raw;
};
union tx_pause_frames_t {
    struct {
        uint32_t txpause:32;	// 0
    };
    uint32_t raw;
};
union tx_single_collision_good_frames_t {
    struct {
        uint32_t txsnglcolg:32;	// 0
    };
    uint32_t raw;
};
union tx_underflow_error_frames_t {
    struct {
        uint32_t txundrflw:32;	// 0
    };
    uint32_t raw;
};
union tx_unicast_frames_good_bad_t {
    struct {
        uint32_t txucastgb:32;	// 0
    };
    uint32_t raw;
};
union tx_vlan_frames_good_t {
    struct {
        uint32_t txvlang:32;	// 0
    };
    uint32_t raw;
};
union vlan_tag_t {
    struct {
        uint32_t vl:16;	// 0
        uint32_t etv:1;	// 16
        uint32_t vtim:1;	// 17
        uint32_t esvl:1;	// 18
        uint32_t vthm:1;	// 19
    };
    uint32_t raw;
};
}
namespace eth0_con_ns {
union eth0_con_t {
    struct {
        uint32_t :8;
        uint32_t clk_rmii:2;	// 8
        uint32_t crs_dv:2;	// 10
        uint32_t :6;
        uint32_t clk_tx:2;	// 18
    };
    uint32_t raw;
};
}
namespace fce_ns {
union id_t {
    struct {
        uint32_t mod_rev:8;	// 0
        uint32_t mod_type:8;	// 8
        uint32_t mod_number:16;	// 16
    };
    uint32_t raw;
};
}
namespace fce_ke_ns {
union cfg_t {
    struct {
        uint32_t cmi:1;	// 0
        uint32_t cei:1;	// 1
        uint32_t lei:1;	// 2
        uint32_t bei:1;	// 3
        uint32_t cce:1;	// 4
        uint32_t alr:1;	// 5
        uint32_t :2;
        uint32_t refin:1;	// 8
        uint32_t refout:1;	// 9
        uint32_t xsel:1;	// 10
    };
    uint32_t raw;
};
union check_t {
    struct {
        uint32_t check:32;	// 0
    };
    uint32_t raw;
};
union crc_t {
    struct {
        uint32_t crc:32;	// 0
    };
    uint32_t raw;
};
union ctr_t {
    struct {
        uint32_t fcm:1;	// 0
        uint32_t frm_cfg:1;	// 1
        uint32_t frm_check:1;	// 2
    };
    uint32_t raw;
};
union ir_t {
    struct {
        uint32_t ir:32;	// 0
    };
    uint32_t raw;
};
union length_t {
    struct {
        uint32_t length:16;	// 0
    };
    uint32_t raw;
};
union res_t {
    struct {
        uint32_t res:32;	// 0
    };
    uint32_t raw;
};
union sts_t {
    struct {
        uint32_t cmf:1;	// 0
        uint32_t cef:1;	// 1
        uint32_t lef:1;	// 2
        uint32_t bef:1;	// 3
    };
    uint32_t raw;
};
}
namespace flash_ns {
union id_t {
    struct {
        uint32_t mod_rev:8;	// 0
        uint32_t mod_type:8;	// 8
        uint32_t mod_number:16;	// 16
    };
    uint32_t raw;
};
}
namespace gpdma0_ns {
union chenreg_t {
    struct {
        uint32_t :8;
        uint32_t we_ch:8;	// 8
    };
    uint32_t raw;
};
union dmacfgreg_t {
    struct {
        uint32_t dma_en:1;	// 0
    };
    uint32_t raw;
};
union lstdstreg_t {
    struct {
        uint32_t :8;
        uint32_t we_ch0:1;	// 8
        uint32_t we_ch1:1;	// 9
        uint32_t we_ch2:1;	// 10
        uint32_t we_ch3:1;	// 11
        uint32_t we_ch4:1;	// 12
        uint32_t we_ch5:1;	// 13
        uint32_t we_ch6:1;	// 14
        uint32_t we_ch7:1;	// 15
    };
    uint32_t raw;
};
union lstsrcreg_t {
    struct {
        uint32_t :8;
        uint32_t we_ch0:1;	// 8
        uint32_t we_ch1:1;	// 9
        uint32_t we_ch2:1;	// 10
        uint32_t we_ch3:1;	// 11
        uint32_t we_ch4:1;	// 12
        uint32_t we_ch5:1;	// 13
        uint32_t we_ch6:1;	// 14
        uint32_t we_ch7:1;	// 15
    };
    uint32_t raw;
};
union maskblock_t {
    struct {
        uint32_t :8;
        uint32_t we_ch0:1;	// 8
        uint32_t we_ch1:1;	// 9
        uint32_t we_ch2:1;	// 10
        uint32_t we_ch3:1;	// 11
        uint32_t we_ch4:1;	// 12
        uint32_t we_ch5:1;	// 13
        uint32_t we_ch6:1;	// 14
        uint32_t we_ch7:1;	// 15
    };
    uint32_t raw;
};
union maskdsttran_t {
    struct {
        uint32_t :8;
        uint32_t we_ch0:1;	// 8
        uint32_t we_ch1:1;	// 9
        uint32_t we_ch2:1;	// 10
        uint32_t we_ch3:1;	// 11
        uint32_t we_ch4:1;	// 12
        uint32_t we_ch5:1;	// 13
        uint32_t we_ch6:1;	// 14
        uint32_t we_ch7:1;	// 15
    };
    uint32_t raw;
};
union maskerr_t {
    struct {
        uint32_t :8;
        uint32_t we_ch0:1;	// 8
        uint32_t we_ch1:1;	// 9
        uint32_t we_ch2:1;	// 10
        uint32_t we_ch3:1;	// 11
        uint32_t we_ch4:1;	// 12
        uint32_t we_ch5:1;	// 13
        uint32_t we_ch6:1;	// 14
        uint32_t we_ch7:1;	// 15
    };
    uint32_t raw;
};
union masksrctran_t {
    struct {
        uint32_t :8;
        uint32_t we_ch0:1;	// 8
        uint32_t we_ch1:1;	// 9
        uint32_t we_ch2:1;	// 10
        uint32_t we_ch3:1;	// 11
        uint32_t we_ch4:1;	// 12
        uint32_t we_ch5:1;	// 13
        uint32_t we_ch6:1;	// 14
        uint32_t we_ch7:1;	// 15
    };
    uint32_t raw;
};
union masktfr_t {
    struct {
        uint32_t :8;
        uint32_t we_ch0:1;	// 8
        uint32_t we_ch1:1;	// 9
        uint32_t we_ch2:1;	// 10
        uint32_t we_ch3:1;	// 11
        uint32_t we_ch4:1;	// 12
        uint32_t we_ch5:1;	// 13
        uint32_t we_ch6:1;	// 14
        uint32_t we_ch7:1;	// 15
    };
    uint32_t raw;
};
union reqdstreg_t {
    struct {
        uint32_t :8;
        uint32_t we_ch0:1;	// 8
        uint32_t we_ch1:1;	// 9
        uint32_t we_ch2:1;	// 10
        uint32_t we_ch3:1;	// 11
        uint32_t we_ch4:1;	// 12
        uint32_t we_ch5:1;	// 13
        uint32_t we_ch6:1;	// 14
        uint32_t we_ch7:1;	// 15
    };
    uint32_t raw;
};
union reqsrcreg_t {
    struct {
        uint32_t :8;
        uint32_t we_ch0:1;	// 8
        uint32_t we_ch1:1;	// 9
        uint32_t we_ch2:1;	// 10
        uint32_t we_ch3:1;	// 11
        uint32_t we_ch4:1;	// 12
        uint32_t we_ch5:1;	// 13
        uint32_t we_ch6:1;	// 14
        uint32_t we_ch7:1;	// 15
    };
    uint32_t raw;
};
union sglreqdstreg_t {
    struct {
        uint32_t :8;
        uint32_t we_ch0:1;	// 8
        uint32_t we_ch1:1;	// 9
        uint32_t we_ch2:1;	// 10
        uint32_t we_ch3:1;	// 11
        uint32_t we_ch4:1;	// 12
        uint32_t we_ch5:1;	// 13
        uint32_t we_ch6:1;	// 14
        uint32_t we_ch7:1;	// 15
    };
    uint32_t raw;
};
union sglreqsrcreg_t {
    struct {
        uint32_t :8;
        uint32_t we_ch0:1;	// 8
        uint32_t we_ch1:1;	// 9
        uint32_t we_ch2:1;	// 10
        uint32_t we_ch3:1;	// 11
        uint32_t we_ch4:1;	// 12
        uint32_t we_ch5:1;	// 13
        uint32_t we_ch6:1;	// 14
        uint32_t we_ch7:1;	// 15
    };
    uint32_t raw;
};
}
namespace gpdma0_ch0_1_ns {
union cfgh_t {
    struct {
        uint32_t fcmode:1;	// 0
        uint32_t fifo_mode:1;	// 1
        uint32_t protctl:3;	// 2
        uint32_t ds_upd_en:1;	// 5
        uint32_t ss_upd_en:1;	// 6
        uint32_t src_per:4;	// 7
        uint32_t dest_per:4;	// 11
    };
    uint32_t raw;
};
union cfgl_t {
    struct {
        uint32_t :5;
        uint32_t ch_prior:3;	// 5
        uint32_t ch_susp:1;	// 8
        uint32_t fifo_empty:1;	// 9
        uint32_t hs_sel_dst:1;	// 10
        uint32_t hs_sel_src:1;	// 11
        uint32_t lock_ch_l:2;	// 12
        uint32_t lock_b_l:2;	// 14
        uint32_t lock_ch:1;	// 16
        uint32_t lock_b:1;	// 17
        uint32_t dst_hs_pol:1;	// 18
        uint32_t src_hs_pol:1;	// 19
        uint32_t max_abrst:10;	// 20
        uint32_t reload_src:1;	// 30
        uint32_t reload_dst:1;	// 31
    };
    uint32_t raw;
};
union ctlh_t {
    struct {
        uint32_t block_ts:12;	// 0
        uint32_t done:1;	// 12
    };
    uint32_t raw;
};
union ctll_t {
    struct {
        uint32_t int_en:1;	// 0
        uint32_t dst_tr_width:3;	// 1
        uint32_t src_tr_width:3;	// 4
        uint32_t dinc:2;	// 7
        uint32_t sinc:2;	// 9
        uint32_t dest_msize:3;	// 11
        uint32_t src_msize:3;	// 14
        uint32_t src_gather_en:1;	// 17
        uint32_t dst_scatter_en:1;	// 18
        uint32_t :1;
        uint32_t tt_fc:3;	// 20
        uint32_t :4;
        uint32_t llp_dst_en:1;	// 27
        uint32_t llp_src_en:1;	// 28
    };
    uint32_t raw;
};
union dar_t {
    struct {
        uint32_t dar:32;	// 0
    };
    uint32_t raw;
};
union dsr_t {
    struct {
        uint32_t dsi:20;	// 0
        uint32_t dsc:12;	// 20
    };
    uint32_t raw;
};
union dstat_t {
    struct {
        uint32_t dstat:32;	// 0
    };
    uint32_t raw;
};
union dstatar_t {
    struct {
        uint32_t dstatar:32;	// 0
    };
    uint32_t raw;
};
union llp_t {
    struct {
        uint32_t :2;
        uint32_t loc:30;	// 2
    };
    uint32_t raw;
};
union sar_t {
    struct {
        uint32_t sar:32;	// 0
    };
    uint32_t raw;
};
union sgr_t {
    struct {
        uint32_t sgi:20;	// 0
        uint32_t sgc:12;	// 20
    };
    uint32_t raw;
};
union sstat_t {
    struct {
        uint32_t sstat:32;	// 0
    };
    uint32_t raw;
};
union sstatar_t {
    struct {
        uint32_t sstatar:32;	// 0
    };
    uint32_t raw;
};
}
namespace hrpwm0_csg_ns {
union blv_t {
    struct {
        uint32_t blv:8;	// 0
    };
    uint32_t raw;
};
union cc_t {
    struct {
        uint32_t ibs:4;	// 0
        uint32_t :4;
        uint32_t imcs:1;	// 8
        uint32_t imcc:2;	// 9
        uint32_t ese:1;	// 11
        uint32_t oie:1;	// 12
        uint32_t ose:1;	// 13
        uint32_t blmc:2;	// 14
        uint32_t ebe:1;	// 16
        uint32_t cofe:1;	// 17
        uint32_t cofm:4;	// 18
        uint32_t :2;
        uint32_t cofc:2;	// 24
    };
    uint32_t raw;
};
union dci_t {
    struct {
        uint32_t svis:4;	// 0
        uint32_t stris:4;	// 4
        uint32_t stpis:4;	// 8
        uint32_t trgis:4;	// 12
        uint32_t stis:4;	// 16
        uint32_t scs:2;	// 20
    };
    uint32_t raw;
};
union dsv1_t {
    struct {
        uint32_t dsv1:10;	// 0
    };
    uint32_t raw;
};
union dsv2_t {
    struct {
        uint32_t dsv2:10;	// 0
    };
    uint32_t raw;
};
union ies_t {
    struct {
        uint32_t svls:2;	// 0
        uint32_t stres:2;	// 2
        uint32_t stpes:2;	// 4
        uint32_t trges:2;	// 6
        uint32_t stes:2;	// 8
    };
    uint32_t raw;
};
union istat_t {
    struct {
        uint32_t vls1s:1;	// 0
        uint32_t vls2s:1;	// 1
        uint32_t trgss:1;	// 2
        uint32_t strss:1;	// 3
        uint32_t stpss:1;	// 4
        uint32_t stds:1;	// 5
        uint32_t crss:1;	// 6
        uint32_t cfss:1;	// 7
        uint32_t cses:1;	// 8
    };
    uint32_t raw;
};
union pc_t {
    struct {
        uint32_t pswv:6;	// 0
    };
    uint32_t raw;
};
union plc_t {
    struct {
        uint32_t ipls:4;	// 0
        uint32_t :4;
        uint32_t plcl:2;	// 8
        uint32_t psl:1;	// 10
        uint32_t plsw:1;	// 11
        uint32_t plec:2;	// 12
        uint32_t plxc:2;	// 14
    };
    uint32_t raw;
};
union sc_t {
    struct {
        uint32_t psrm:2;	// 0
        uint32_t pstm:2;	// 2
        uint32_t fpd:1;	// 4
        uint32_t psv:2;	// 5
        uint32_t :1;
        uint32_t scm:2;	// 8
        uint32_t ssrm:2;	// 10
        uint32_t sstm:2;	// 12
        uint32_t svsc:2;	// 14
        uint32_t swsm:2;	// 16
        uint32_t gcfg:2;	// 18
        uint32_t ist:1;	// 20
        uint32_t pse:1;	// 21
        uint32_t :2;
        uint32_t pswm:2;	// 24
    };
    uint32_t raw;
};
union sdsv1_t {
    struct {
        uint32_t sdsv1:10;	// 0
    };
    uint32_t raw;
};
union spc_t {
    struct {
        uint32_t spswv:6;	// 0
    };
    uint32_t raw;
};
union sre_t {
    struct {
        uint32_t vls1e:1;	// 0
        uint32_t vls2e:1;	// 1
        uint32_t trgse:1;	// 2
        uint32_t strse:1;	// 3
        uint32_t stpse:1;	// 4
        uint32_t stde:1;	// 5
        uint32_t crse:1;	// 6
        uint32_t cfse:1;	// 7
        uint32_t csee:1;	// 8
    };
    uint32_t raw;
};
union srs_t {
    struct {
        uint32_t vls1s:2;	// 0
        uint32_t vls2s:2;	// 2
        uint32_t trls:2;	// 4
        uint32_t ssls:2;	// 6
        uint32_t stls:2;	// 8
        uint32_t crfls:2;	// 10
        uint32_t csls:2;	// 12
    };
    uint32_t raw;
};
union swc_t {
    struct {
        uint32_t cvls1:1;	// 0
        uint32_t cvls2:1;	// 1
        uint32_t ctrgs:1;	// 2
        uint32_t cstrs:1;	// 3
        uint32_t cstps:1;	// 4
        uint32_t cstd:1;	// 5
        uint32_t ccrs:1;	// 6
        uint32_t ccfs:1;	// 7
        uint32_t ccss:1;	// 8
    };
    uint32_t raw;
};
union sws_t {
    struct {
        uint32_t svls1:1;	// 0
        uint32_t svls2:1;	// 1
        uint32_t strgs:1;	// 2
        uint32_t sstrs:1;	// 3
        uint32_t sstps:1;	// 4
        uint32_t sstd:1;	// 5
        uint32_t scrs:1;	// 6
        uint32_t scfs:1;	// 7
        uint32_t scss:1;	// 8
    };
    uint32_t raw;
};
}
namespace hrpwm0_csg0_ns {
union blv_t {
    struct {
        uint32_t blv:8;	// 0
    };
    uint32_t raw;
};
union cc_t {
    struct {
        uint32_t ibs:4;	// 0
        uint32_t :4;
        uint32_t imcs:1;	// 8
        uint32_t imcc:2;	// 9
        uint32_t ese:1;	// 11
        uint32_t oie:1;	// 12
        uint32_t ose:1;	// 13
        uint32_t blmc:2;	// 14
        uint32_t ebe:1;	// 16
        uint32_t cofe:1;	// 17
        uint32_t cofm:4;	// 18
        uint32_t :2;
        uint32_t cofc:2;	// 24
    };
    uint32_t raw;
};
union dci_t {
    struct {
        uint32_t svis:4;	// 0
        uint32_t stris:4;	// 4
        uint32_t stpis:4;	// 8
        uint32_t trgis:4;	// 12
        uint32_t stis:4;	// 16
        uint32_t scs:2;	// 20
    };
    uint32_t raw;
};
union dsv1_t {
    struct {
        uint32_t dsv1:10;	// 0
    };
    uint32_t raw;
};
union dsv2_t {
    struct {
        uint32_t dsv2:10;	// 0
    };
    uint32_t raw;
};
union ies_t {
    struct {
        uint32_t svls:2;	// 0
        uint32_t stres:2;	// 2
        uint32_t stpes:2;	// 4
        uint32_t trges:2;	// 6
        uint32_t stes:2;	// 8
    };
    uint32_t raw;
};
union istat_t {
    struct {
        uint32_t vls1s:1;	// 0
        uint32_t vls2s:1;	// 1
        uint32_t trgss:1;	// 2
        uint32_t strss:1;	// 3
        uint32_t stpss:1;	// 4
        uint32_t stds:1;	// 5
        uint32_t crss:1;	// 6
        uint32_t cfss:1;	// 7
        uint32_t cses:1;	// 8
    };
    uint32_t raw;
};
union pc_t {
    struct {
        uint32_t pswv:6;	// 0
    };
    uint32_t raw;
};
union plc_t {
    struct {
        uint32_t ipls:4;	// 0
        uint32_t :4;
        uint32_t plcl:2;	// 8
        uint32_t psl:1;	// 10
        uint32_t plsw:1;	// 11
        uint32_t plec:2;	// 12
        uint32_t plxc:2;	// 14
    };
    uint32_t raw;
};
union sc_t {
    struct {
        uint32_t psrm:2;	// 0
        uint32_t pstm:2;	// 2
        uint32_t fpd:1;	// 4
        uint32_t psv:2;	// 5
        uint32_t :1;
        uint32_t scm:2;	// 8
        uint32_t ssrm:2;	// 10
        uint32_t sstm:2;	// 12
        uint32_t svsc:2;	// 14
        uint32_t swsm:2;	// 16
        uint32_t gcfg:2;	// 18
        uint32_t ist:1;	// 20
        uint32_t pse:1;	// 21
        uint32_t :2;
        uint32_t pswm:2;	// 24
    };
    uint32_t raw;
};
union sdsv1_t {
    struct {
        uint32_t sdsv1:10;	// 0
    };
    uint32_t raw;
};
union spc_t {
    struct {
        uint32_t spswv:6;	// 0
    };
    uint32_t raw;
};
union sre_t {
    struct {
        uint32_t vls1e:1;	// 0
        uint32_t vls2e:1;	// 1
        uint32_t trgse:1;	// 2
        uint32_t strse:1;	// 3
        uint32_t stpse:1;	// 4
        uint32_t stde:1;	// 5
        uint32_t crse:1;	// 6
        uint32_t cfse:1;	// 7
        uint32_t csee:1;	// 8
    };
    uint32_t raw;
};
union srs_t {
    struct {
        uint32_t vls1s:2;	// 0
        uint32_t vls2s:2;	// 2
        uint32_t trls:2;	// 4
        uint32_t ssls:2;	// 6
        uint32_t stls:2;	// 8
        uint32_t crfls:2;	// 10
        uint32_t csls:2;	// 12
    };
    uint32_t raw;
};
union swc_t {
    struct {
        uint32_t cvls1:1;	// 0
        uint32_t cvls2:1;	// 1
        uint32_t ctrgs:1;	// 2
        uint32_t cstrs:1;	// 3
        uint32_t cstps:1;	// 4
        uint32_t cstd:1;	// 5
        uint32_t ccrs:1;	// 6
        uint32_t ccfs:1;	// 7
        uint32_t ccss:1;	// 8
    };
    uint32_t raw;
};
union sws_t {
    struct {
        uint32_t svls1:1;	// 0
        uint32_t svls2:1;	// 1
        uint32_t strgs:1;	// 2
        uint32_t sstrs:1;	// 3
        uint32_t sstps:1;	// 4
        uint32_t sstd:1;	// 5
        uint32_t scrs:1;	// 6
        uint32_t scfs:1;	// 7
        uint32_t scss:1;	// 8
    };
    uint32_t raw;
};
}
namespace hrpwm0_csg1_ns {
union blv_t {
    struct {
        uint32_t blv:8;	// 0
    };
    uint32_t raw;
};
union cc_t {
    struct {
        uint32_t ibs:4;	// 0
        uint32_t :4;
        uint32_t imcs:1;	// 8
        uint32_t imcc:2;	// 9
        uint32_t ese:1;	// 11
        uint32_t oie:1;	// 12
        uint32_t ose:1;	// 13
        uint32_t blmc:2;	// 14
        uint32_t ebe:1;	// 16
        uint32_t cofe:1;	// 17
        uint32_t cofm:4;	// 18
        uint32_t :2;
        uint32_t cofc:2;	// 24
    };
    uint32_t raw;
};
union dci_t {
    struct {
        uint32_t svis:4;	// 0
        uint32_t stris:4;	// 4
        uint32_t stpis:4;	// 8
        uint32_t trgis:4;	// 12
        uint32_t stis:4;	// 16
        uint32_t scs:2;	// 20
    };
    uint32_t raw;
};
union dsv1_t {
    struct {
        uint32_t dsv1:10;	// 0
    };
    uint32_t raw;
};
union dsv2_t {
    struct {
        uint32_t dsv2:10;	// 0
    };
    uint32_t raw;
};
union ies_t {
    struct {
        uint32_t svls:2;	// 0
        uint32_t stres:2;	// 2
        uint32_t stpes:2;	// 4
        uint32_t trges:2;	// 6
        uint32_t stes:2;	// 8
    };
    uint32_t raw;
};
union istat_t {
    struct {
        uint32_t vls1s:1;	// 0
        uint32_t vls2s:1;	// 1
        uint32_t trgss:1;	// 2
        uint32_t strss:1;	// 3
        uint32_t stpss:1;	// 4
        uint32_t stds:1;	// 5
        uint32_t crss:1;	// 6
        uint32_t cfss:1;	// 7
        uint32_t cses:1;	// 8
    };
    uint32_t raw;
};
union pc_t {
    struct {
        uint32_t pswv:6;	// 0
    };
    uint32_t raw;
};
union plc_t {
    struct {
        uint32_t ipls:4;	// 0
        uint32_t :4;
        uint32_t plcl:2;	// 8
        uint32_t psl:1;	// 10
        uint32_t plsw:1;	// 11
        uint32_t plec:2;	// 12
        uint32_t plxc:2;	// 14
    };
    uint32_t raw;
};
union sc_t {
    struct {
        uint32_t psrm:2;	// 0
        uint32_t pstm:2;	// 2
        uint32_t fpd:1;	// 4
        uint32_t psv:2;	// 5
        uint32_t :1;
        uint32_t scm:2;	// 8
        uint32_t ssrm:2;	// 10
        uint32_t sstm:2;	// 12
        uint32_t svsc:2;	// 14
        uint32_t swsm:2;	// 16
        uint32_t gcfg:2;	// 18
        uint32_t ist:1;	// 20
        uint32_t pse:1;	// 21
        uint32_t :2;
        uint32_t pswm:2;	// 24
    };
    uint32_t raw;
};
union sdsv1_t {
    struct {
        uint32_t sdsv1:10;	// 0
    };
    uint32_t raw;
};
union spc_t {
    struct {
        uint32_t spswv:6;	// 0
    };
    uint32_t raw;
};
union sre_t {
    struct {
        uint32_t vls1e:1;	// 0
        uint32_t vls2e:1;	// 1
        uint32_t trgse:1;	// 2
        uint32_t strse:1;	// 3
        uint32_t stpse:1;	// 4
        uint32_t stde:1;	// 5
        uint32_t crse:1;	// 6
        uint32_t cfse:1;	// 7
        uint32_t csee:1;	// 8
    };
    uint32_t raw;
};
union srs_t {
    struct {
        uint32_t vls1s:2;	// 0
        uint32_t vls2s:2;	// 2
        uint32_t trls:2;	// 4
        uint32_t ssls:2;	// 6
        uint32_t stls:2;	// 8
        uint32_t crfls:2;	// 10
        uint32_t csls:2;	// 12
    };
    uint32_t raw;
};
union swc_t {
    struct {
        uint32_t cvls1:1;	// 0
        uint32_t cvls2:1;	// 1
        uint32_t ctrgs:1;	// 2
        uint32_t cstrs:1;	// 3
        uint32_t cstps:1;	// 4
        uint32_t cstd:1;	// 5
        uint32_t ccrs:1;	// 6
        uint32_t ccfs:1;	// 7
        uint32_t ccss:1;	// 8
    };
    uint32_t raw;
};
union sws_t {
    struct {
        uint32_t svls1:1;	// 0
        uint32_t svls2:1;	// 1
        uint32_t strgs:1;	// 2
        uint32_t sstrs:1;	// 3
        uint32_t sstps:1;	// 4
        uint32_t sstd:1;	// 5
        uint32_t scrs:1;	// 6
        uint32_t scfs:1;	// 7
        uint32_t scss:1;	// 8
    };
    uint32_t raw;
};
}
namespace hrpwm0_csg2_ns {
union blv_t {
    struct {
        uint32_t blv:8;	// 0
    };
    uint32_t raw;
};
union cc_t {
    struct {
        uint32_t ibs:4;	// 0
        uint32_t :4;
        uint32_t imcs:1;	// 8
        uint32_t imcc:2;	// 9
        uint32_t ese:1;	// 11
        uint32_t oie:1;	// 12
        uint32_t ose:1;	// 13
        uint32_t blmc:2;	// 14
        uint32_t ebe:1;	// 16
        uint32_t cofe:1;	// 17
        uint32_t cofm:4;	// 18
        uint32_t :2;
        uint32_t cofc:2;	// 24
    };
    uint32_t raw;
};
union dci_t {
    struct {
        uint32_t svis:4;	// 0
        uint32_t stris:4;	// 4
        uint32_t stpis:4;	// 8
        uint32_t trgis:4;	// 12
        uint32_t stis:4;	// 16
        uint32_t scs:2;	// 20
    };
    uint32_t raw;
};
union dsv1_t {
    struct {
        uint32_t dsv1:10;	// 0
    };
    uint32_t raw;
};
union dsv2_t {
    struct {
        uint32_t dsv2:10;	// 0
    };
    uint32_t raw;
};
union ies_t {
    struct {
        uint32_t svls:2;	// 0
        uint32_t stres:2;	// 2
        uint32_t stpes:2;	// 4
        uint32_t trges:2;	// 6
        uint32_t stes:2;	// 8
    };
    uint32_t raw;
};
union istat_t {
    struct {
        uint32_t vls1s:1;	// 0
        uint32_t vls2s:1;	// 1
        uint32_t trgss:1;	// 2
        uint32_t strss:1;	// 3
        uint32_t stpss:1;	// 4
        uint32_t stds:1;	// 5
        uint32_t crss:1;	// 6
        uint32_t cfss:1;	// 7
        uint32_t cses:1;	// 8
    };
    uint32_t raw;
};
union pc_t {
    struct {
        uint32_t pswv:6;	// 0
    };
    uint32_t raw;
};
union plc_t {
    struct {
        uint32_t ipls:4;	// 0
        uint32_t :4;
        uint32_t plcl:2;	// 8
        uint32_t psl:1;	// 10
        uint32_t plsw:1;	// 11
        uint32_t plec:2;	// 12
        uint32_t plxc:2;	// 14
    };
    uint32_t raw;
};
union sc_t {
    struct {
        uint32_t psrm:2;	// 0
        uint32_t pstm:2;	// 2
        uint32_t fpd:1;	// 4
        uint32_t psv:2;	// 5
        uint32_t :1;
        uint32_t scm:2;	// 8
        uint32_t ssrm:2;	// 10
        uint32_t sstm:2;	// 12
        uint32_t svsc:2;	// 14
        uint32_t swsm:2;	// 16
        uint32_t gcfg:2;	// 18
        uint32_t ist:1;	// 20
        uint32_t pse:1;	// 21
        uint32_t :2;
        uint32_t pswm:2;	// 24
    };
    uint32_t raw;
};
union sdsv1_t {
    struct {
        uint32_t sdsv1:10;	// 0
    };
    uint32_t raw;
};
union spc_t {
    struct {
        uint32_t spswv:6;	// 0
    };
    uint32_t raw;
};
union sre_t {
    struct {
        uint32_t vls1e:1;	// 0
        uint32_t vls2e:1;	// 1
        uint32_t trgse:1;	// 2
        uint32_t strse:1;	// 3
        uint32_t stpse:1;	// 4
        uint32_t stde:1;	// 5
        uint32_t crse:1;	// 6
        uint32_t cfse:1;	// 7
        uint32_t csee:1;	// 8
    };
    uint32_t raw;
};
union srs_t {
    struct {
        uint32_t vls1s:2;	// 0
        uint32_t vls2s:2;	// 2
        uint32_t trls:2;	// 4
        uint32_t ssls:2;	// 6
        uint32_t stls:2;	// 8
        uint32_t crfls:2;	// 10
        uint32_t csls:2;	// 12
    };
    uint32_t raw;
};
union swc_t {
    struct {
        uint32_t cvls1:1;	// 0
        uint32_t cvls2:1;	// 1
        uint32_t ctrgs:1;	// 2
        uint32_t cstrs:1;	// 3
        uint32_t cstps:1;	// 4
        uint32_t cstd:1;	// 5
        uint32_t ccrs:1;	// 6
        uint32_t ccfs:1;	// 7
        uint32_t ccss:1;	// 8
    };
    uint32_t raw;
};
union sws_t {
    struct {
        uint32_t svls1:1;	// 0
        uint32_t svls2:1;	// 1
        uint32_t strgs:1;	// 2
        uint32_t sstrs:1;	// 3
        uint32_t sstps:1;	// 4
        uint32_t sstd:1;	// 5
        uint32_t scrs:1;	// 6
        uint32_t scfs:1;	// 7
        uint32_t scss:1;	// 8
    };
    uint32_t raw;
};
}
namespace hrpwm0_hrc_ns {
union cr1_t {
    struct {
        uint32_t cr1:8;	// 0
    };
    uint32_t raw;
};
union cr2_t {
    struct {
        uint32_t cr2:8;	// 0
    };
    uint32_t raw;
};
union dcf_t {
    struct {
        uint32_t dtfv:16;	// 0
    };
    uint32_t raw;
};
union dcr_t {
    struct {
        uint32_t dtrv:16;	// 0
    };
    uint32_t raw;
};
union gc_t {
    struct {
        uint32_t hrm0:2;	// 0
        uint32_t hrm1:2;	// 2
        uint32_t :4;
        uint32_t dte:1;	// 8
        uint32_t tr0e:1;	// 9
        uint32_t tr1e:1;	// 10
        uint32_t stc:1;	// 11
        uint32_t dstc:1;	// 12
        uint32_t ocs0:1;	// 13
        uint32_t ocs1:1;	// 14
        uint32_t :1;
        uint32_t dtus:1;	// 16
    };
    uint32_t raw;
};
union gsel_t {
    struct {
        uint32_t c0ss:3;	// 0
        uint32_t c0cs:3;	// 3
        uint32_t s0m:2;	// 6
        uint32_t c0m:2;	// 8
        uint32_t s0es:2;	// 10
        uint32_t c0es:2;	// 12
        uint32_t :2;
        uint32_t c1ss:3;	// 16
        uint32_t c1cs:3;	// 19
        uint32_t s1m:2;	// 22
        uint32_t c1m:2;	// 24
        uint32_t s1es:2;	// 26
        uint32_t c1es:2;	// 28
    };
    uint32_t raw;
};
union pl_t {
    struct {
        uint32_t psl0:1;	// 0
        uint32_t psl1:1;	// 1
    };
    uint32_t raw;
};
union sc_t {
    struct {
        uint32_t st:1;	// 0
    };
    uint32_t raw;
};
union scr1_t {
    struct {
        uint32_t scr1:8;	// 0
    };
    uint32_t raw;
};
union scr2_t {
    struct {
        uint32_t scr2:8;	// 0
    };
    uint32_t raw;
};
union sdcf_t {
    struct {
        uint32_t sdtfv:16;	// 0
    };
    uint32_t raw;
};
union sdcr_t {
    struct {
        uint32_t sdtrv:16;	// 0
    };
    uint32_t raw;
};
union ssc_t {
    struct {
        uint32_t sst:1;	// 0
    };
    uint32_t raw;
};
union tsel_t {
    struct {
        uint32_t tsel0:3;	// 0
        uint32_t tsel1:3;	// 3
        uint32_t :10;
        uint32_t ts0e:1;	// 16
        uint32_t ts1e:1;	// 17
    };
    uint32_t raw;
};
}
namespace hrpwm0_hrc0_ns {
union cr1_t {
    struct {
        uint32_t cr1:8;	// 0
    };
    uint32_t raw;
};
union cr2_t {
    struct {
        uint32_t cr2:8;	// 0
    };
    uint32_t raw;
};
union dcf_t {
    struct {
        uint32_t dtfv:16;	// 0
    };
    uint32_t raw;
};
union dcr_t {
    struct {
        uint32_t dtrv:16;	// 0
    };
    uint32_t raw;
};
union gc_t {
    struct {
        uint32_t hrm0:2;	// 0
        uint32_t hrm1:2;	// 2
        uint32_t :4;
        uint32_t dte:1;	// 8
        uint32_t tr0e:1;	// 9
        uint32_t tr1e:1;	// 10
        uint32_t stc:1;	// 11
        uint32_t dstc:1;	// 12
        uint32_t ocs0:1;	// 13
        uint32_t ocs1:1;	// 14
        uint32_t :1;
        uint32_t dtus:1;	// 16
    };
    uint32_t raw;
};
union gsel_t {
    struct {
        uint32_t c0ss:3;	// 0
        uint32_t c0cs:3;	// 3
        uint32_t s0m:2;	// 6
        uint32_t c0m:2;	// 8
        uint32_t s0es:2;	// 10
        uint32_t c0es:2;	// 12
        uint32_t :2;
        uint32_t c1ss:3;	// 16
        uint32_t c1cs:3;	// 19
        uint32_t s1m:2;	// 22
        uint32_t c1m:2;	// 24
        uint32_t s1es:2;	// 26
        uint32_t c1es:2;	// 28
    };
    uint32_t raw;
};
union pl_t {
    struct {
        uint32_t psl0:1;	// 0
        uint32_t psl1:1;	// 1
    };
    uint32_t raw;
};
union sc_t {
    struct {
        uint32_t st:1;	// 0
    };
    uint32_t raw;
};
union scr1_t {
    struct {
        uint32_t scr1:8;	// 0
    };
    uint32_t raw;
};
union scr2_t {
    struct {
        uint32_t scr2:8;	// 0
    };
    uint32_t raw;
};
union sdcf_t {
    struct {
        uint32_t sdtfv:16;	// 0
    };
    uint32_t raw;
};
union sdcr_t {
    struct {
        uint32_t sdtrv:16;	// 0
    };
    uint32_t raw;
};
union ssc_t {
    struct {
        uint32_t sst:1;	// 0
    };
    uint32_t raw;
};
union tsel_t {
    struct {
        uint32_t tsel0:3;	// 0
        uint32_t tsel1:3;	// 3
        uint32_t :10;
        uint32_t ts0e:1;	// 16
        uint32_t ts1e:1;	// 17
    };
    uint32_t raw;
};
}
namespace hrpwm0_hrc1_ns {
union cr1_t {
    struct {
        uint32_t cr1:8;	// 0
    };
    uint32_t raw;
};
union cr2_t {
    struct {
        uint32_t cr2:8;	// 0
    };
    uint32_t raw;
};
union dcf_t {
    struct {
        uint32_t dtfv:16;	// 0
    };
    uint32_t raw;
};
union dcr_t {
    struct {
        uint32_t dtrv:16;	// 0
    };
    uint32_t raw;
};
union gc_t {
    struct {
        uint32_t hrm0:2;	// 0
        uint32_t hrm1:2;	// 2
        uint32_t :4;
        uint32_t dte:1;	// 8
        uint32_t tr0e:1;	// 9
        uint32_t tr1e:1;	// 10
        uint32_t stc:1;	// 11
        uint32_t dstc:1;	// 12
        uint32_t ocs0:1;	// 13
        uint32_t ocs1:1;	// 14
        uint32_t :1;
        uint32_t dtus:1;	// 16
    };
    uint32_t raw;
};
union gsel_t {
    struct {
        uint32_t c0ss:3;	// 0
        uint32_t c0cs:3;	// 3
        uint32_t s0m:2;	// 6
        uint32_t c0m:2;	// 8
        uint32_t s0es:2;	// 10
        uint32_t c0es:2;	// 12
        uint32_t :2;
        uint32_t c1ss:3;	// 16
        uint32_t c1cs:3;	// 19
        uint32_t s1m:2;	// 22
        uint32_t c1m:2;	// 24
        uint32_t s1es:2;	// 26
        uint32_t c1es:2;	// 28
    };
    uint32_t raw;
};
union pl_t {
    struct {
        uint32_t psl0:1;	// 0
        uint32_t psl1:1;	// 1
    };
    uint32_t raw;
};
union sc_t {
    struct {
        uint32_t st:1;	// 0
    };
    uint32_t raw;
};
union scr1_t {
    struct {
        uint32_t scr1:8;	// 0
    };
    uint32_t raw;
};
union scr2_t {
    struct {
        uint32_t scr2:8;	// 0
    };
    uint32_t raw;
};
union sdcf_t {
    struct {
        uint32_t sdtfv:16;	// 0
    };
    uint32_t raw;
};
union sdcr_t {
    struct {
        uint32_t sdtrv:16;	// 0
    };
    uint32_t raw;
};
union ssc_t {
    struct {
        uint32_t sst:1;	// 0
    };
    uint32_t raw;
};
union tsel_t {
    struct {
        uint32_t tsel0:3;	// 0
        uint32_t tsel1:3;	// 3
        uint32_t :10;
        uint32_t ts0e:1;	// 16
        uint32_t ts1e:1;	// 17
    };
    uint32_t raw;
};
}
namespace hrpwm0_hrc2_ns {
union cr1_t {
    struct {
        uint32_t cr1:8;	// 0
    };
    uint32_t raw;
};
union cr2_t {
    struct {
        uint32_t cr2:8;	// 0
    };
    uint32_t raw;
};
union dcf_t {
    struct {
        uint32_t dtfv:16;	// 0
    };
    uint32_t raw;
};
union dcr_t {
    struct {
        uint32_t dtrv:16;	// 0
    };
    uint32_t raw;
};
union gc_t {
    struct {
        uint32_t hrm0:2;	// 0
        uint32_t hrm1:2;	// 2
        uint32_t :4;
        uint32_t dte:1;	// 8
        uint32_t tr0e:1;	// 9
        uint32_t tr1e:1;	// 10
        uint32_t stc:1;	// 11
        uint32_t dstc:1;	// 12
        uint32_t ocs0:1;	// 13
        uint32_t ocs1:1;	// 14
        uint32_t :1;
        uint32_t dtus:1;	// 16
    };
    uint32_t raw;
};
union gsel_t {
    struct {
        uint32_t c0ss:3;	// 0
        uint32_t c0cs:3;	// 3
        uint32_t s0m:2;	// 6
        uint32_t c0m:2;	// 8
        uint32_t s0es:2;	// 10
        uint32_t c0es:2;	// 12
        uint32_t :2;
        uint32_t c1ss:3;	// 16
        uint32_t c1cs:3;	// 19
        uint32_t s1m:2;	// 22
        uint32_t c1m:2;	// 24
        uint32_t s1es:2;	// 26
        uint32_t c1es:2;	// 28
    };
    uint32_t raw;
};
union pl_t {
    struct {
        uint32_t psl0:1;	// 0
        uint32_t psl1:1;	// 1
    };
    uint32_t raw;
};
union sc_t {
    struct {
        uint32_t st:1;	// 0
    };
    uint32_t raw;
};
union scr1_t {
    struct {
        uint32_t scr1:8;	// 0
    };
    uint32_t raw;
};
union scr2_t {
    struct {
        uint32_t scr2:8;	// 0
    };
    uint32_t raw;
};
union sdcf_t {
    struct {
        uint32_t sdtfv:16;	// 0
    };
    uint32_t raw;
};
union sdcr_t {
    struct {
        uint32_t sdtrv:16;	// 0
    };
    uint32_t raw;
};
union ssc_t {
    struct {
        uint32_t sst:1;	// 0
    };
    uint32_t raw;
};
union tsel_t {
    struct {
        uint32_t tsel0:3;	// 0
        uint32_t tsel1:3;	// 3
        uint32_t :10;
        uint32_t ts0e:1;	// 16
        uint32_t ts1e:1;	// 17
    };
    uint32_t raw;
};
}
namespace hrpwm0_hrc3_ns {
union cr1_t {
    struct {
        uint32_t cr1:8;	// 0
    };
    uint32_t raw;
};
union cr2_t {
    struct {
        uint32_t cr2:8;	// 0
    };
    uint32_t raw;
};
union dcf_t {
    struct {
        uint32_t dtfv:16;	// 0
    };
    uint32_t raw;
};
union dcr_t {
    struct {
        uint32_t dtrv:16;	// 0
    };
    uint32_t raw;
};
union gc_t {
    struct {
        uint32_t hrm0:2;	// 0
        uint32_t hrm1:2;	// 2
        uint32_t :4;
        uint32_t dte:1;	// 8
        uint32_t tr0e:1;	// 9
        uint32_t tr1e:1;	// 10
        uint32_t stc:1;	// 11
        uint32_t dstc:1;	// 12
        uint32_t ocs0:1;	// 13
        uint32_t ocs1:1;	// 14
        uint32_t :1;
        uint32_t dtus:1;	// 16
    };
    uint32_t raw;
};
union gsel_t {
    struct {
        uint32_t c0ss:3;	// 0
        uint32_t c0cs:3;	// 3
        uint32_t s0m:2;	// 6
        uint32_t c0m:2;	// 8
        uint32_t s0es:2;	// 10
        uint32_t c0es:2;	// 12
        uint32_t :2;
        uint32_t c1ss:3;	// 16
        uint32_t c1cs:3;	// 19
        uint32_t s1m:2;	// 22
        uint32_t c1m:2;	// 24
        uint32_t s1es:2;	// 26
        uint32_t c1es:2;	// 28
    };
    uint32_t raw;
};
union pl_t {
    struct {
        uint32_t psl0:1;	// 0
        uint32_t psl1:1;	// 1
    };
    uint32_t raw;
};
union sc_t {
    struct {
        uint32_t st:1;	// 0
    };
    uint32_t raw;
};
union scr1_t {
    struct {
        uint32_t scr1:8;	// 0
    };
    uint32_t raw;
};
union scr2_t {
    struct {
        uint32_t scr2:8;	// 0
    };
    uint32_t raw;
};
union sdcf_t {
    struct {
        uint32_t sdtfv:16;	// 0
    };
    uint32_t raw;
};
union sdcr_t {
    struct {
        uint32_t sdtrv:16;	// 0
    };
    uint32_t raw;
};
union ssc_t {
    struct {
        uint32_t sst:1;	// 0
    };
    uint32_t raw;
};
union tsel_t {
    struct {
        uint32_t tsel0:3;	// 0
        uint32_t tsel1:3;	// 3
        uint32_t :10;
        uint32_t ts0e:1;	// 16
        uint32_t ts1e:1;	// 17
    };
    uint32_t raw;
};
}
namespace ledts_ns {
union fnctl_t {
    struct {
        uint32_t :25;
        uint32_t nr_tsin:3;	// 25
        uint32_t :1;
        uint32_t nr_ledcol:3;	// 29
    };
    uint32_t raw;
};
union globctl_t {
    struct {
        uint32_t ts_en:1;	// 0
        uint32_t ld_en:1;	// 1
        uint32_t :11;
        uint32_t its_en:1;	// 13
        uint32_t itf_en:1;	// 14
        uint32_t itp_en:1;	// 15
        uint32_t clk_ps:16;	// 16
    };
    uint32_t raw;
};
union id_t {
    struct {
        uint32_t mod_rev:8;	// 0
        uint32_t mod_type:8;	// 8
        uint32_t mod_number:16;	// 16
    };
    uint32_t raw;
};
union ldcmp0_t {
    struct {
        uint32_t cmp_ld0:8;	// 0
        uint32_t cmp_ld1:8;	// 8
        uint32_t cmp_ld2:8;	// 16
        uint32_t cmp_ld3:8;	// 24
    };
    uint32_t raw;
};
union ldcmp1_t {
    struct {
        uint32_t cmp_ld4:8;	// 0
        uint32_t cmp_ld5:8;	// 8
        uint32_t cmp_ld6:8;	// 16
        uint32_t cmp_lda_tscom:8;	// 24
    };
    uint32_t raw;
};
union line0_t {
    struct {
        uint32_t line_0:8;	// 0
        uint32_t line_1:8;	// 8
        uint32_t line_2:8;	// 16
        uint32_t line_3:8;	// 24
    };
    uint32_t raw;
};
union line1_t {
    struct {
        uint32_t line_4:8;	// 0
        uint32_t line_5:8;	// 8
        uint32_t line_6:8;	// 16
        uint32_t line_a:8;	// 24
    };
    uint32_t raw;
};
union tscmp0_t {
    struct {
        uint32_t cmp_ts0:8;	// 0
        uint32_t cmp_ts1:8;	// 8
        uint32_t cmp_ts2:8;	// 16
        uint32_t cmp_ts3:8;	// 24
    };
    uint32_t raw;
};
union tscmp1_t {
    struct {
        uint32_t cmp_ts4:8;	// 0
        uint32_t cmp_ts5:8;	// 8
        uint32_t cmp_ts6:8;	// 16
        uint32_t cmp_ts7:8;	// 24
    };
    uint32_t raw;
};
}
namespace pmu_ns {
union id_t {
    struct {
        uint32_t mod_rev:8;	// 0
        uint32_t mod_type:8;	// 8
        uint32_t mod_number:16;	// 16
    };
    uint32_t raw;
};
}
namespace ppb_ns {
union ccr_t {
    struct {
        uint32_t :3;
        uint32_t unalign_trp:1;	// 3
        uint32_t div_0_trp:1;	// 4
    };
    uint32_t raw;
};
union mpu_ctrl_t {
    struct {
        uint32_t enable:1;	// 0
        uint32_t hfnmiena:1;	// 1
        uint32_t privdefena:1;	// 2
    };
    uint32_t raw;
};
union mpu_rasr_t {
    struct {
        uint32_t enable:1;	// 0
        uint32_t size:5;	// 1
        uint32_t :2;
        uint32_t srd:8;	// 8
        uint32_t b:1;	// 16
        uint32_t c:1;	// 17
        uint32_t s:1;	// 18
        uint32_t tex:3;	// 19
        uint32_t :2;
        uint32_t ap:3;	// 24
        uint32_t :1;
        uint32_t xn:1;	// 28
    };
    uint32_t raw;
};
union mpu_rasr_a1_t {
    struct {
        uint32_t enable:1;	// 0
        uint32_t size:5;	// 1
        uint32_t :2;
        uint32_t srd:8;	// 8
        uint32_t b:1;	// 16
        uint32_t c:1;	// 17
        uint32_t s:1;	// 18
        uint32_t tex:3;	// 19
        uint32_t :2;
        uint32_t ap:3;	// 24
        uint32_t :1;
        uint32_t xn:1;	// 28
    };
    uint32_t raw;
};
union mpu_rasr_a2_t {
    struct {
        uint32_t enable:1;	// 0
        uint32_t size:5;	// 1
        uint32_t :2;
        uint32_t srd:8;	// 8
        uint32_t b:1;	// 16
        uint32_t c:1;	// 17
        uint32_t s:1;	// 18
        uint32_t tex:3;	// 19
        uint32_t :2;
        uint32_t ap:3;	// 24
        uint32_t :1;
        uint32_t xn:1;	// 28
    };
    uint32_t raw;
};
union mpu_rasr_a3_t {
    struct {
        uint32_t enable:1;	// 0
        uint32_t size:5;	// 1
        uint32_t :2;
        uint32_t srd:8;	// 8
        uint32_t b:1;	// 16
        uint32_t c:1;	// 17
        uint32_t s:1;	// 18
        uint32_t tex:3;	// 19
        uint32_t :2;
        uint32_t ap:3;	// 24
        uint32_t :1;
        uint32_t xn:1;	// 28
    };
    uint32_t raw;
};
union mpu_rbar_t {
    struct {
        uint32_t region:4;	// 0
        uint32_t valid:1;	// 4
        uint32_t :4;
        uint32_t addr:23;	// 9
    };
    uint32_t raw;
};
union mpu_rbar_a1_t {
    struct {
        uint32_t region:4;	// 0
        uint32_t valid:1;	// 4
        uint32_t :4;
        uint32_t addr:23;	// 9
    };
    uint32_t raw;
};
union mpu_rbar_a2_t {
    struct {
        uint32_t region:4;	// 0
        uint32_t valid:1;	// 4
        uint32_t :4;
        uint32_t addr:23;	// 9
    };
    uint32_t raw;
};
union mpu_rbar_a3_t {
    struct {
        uint32_t region:4;	// 0
        uint32_t valid:1;	// 4
        uint32_t :4;
        uint32_t addr:23;	// 9
    };
    uint32_t raw;
};
union mpu_rnr_t {
    struct {
        uint32_t region:8;	// 0
    };
    uint32_t raw;
};
union mpu_type_t {
    struct {
        uint32_t separate:1;	// 0
        uint32_t :7;
        uint32_t dregion:8;	// 8
        uint32_t iregion:8;	// 16
    };
    uint32_t raw;
};
union nvic_iabr0_t {
    struct {
        uint32_t active:32;	// 0
    };
    uint32_t raw;
};
union nvic_iabr1_t {
    struct {
        uint32_t active:32;	// 0
    };
    uint32_t raw;
};
union nvic_iabr2_t {
    struct {
        uint32_t active:32;	// 0
    };
    uint32_t raw;
};
union nvic_iabr3_t {
    struct {
        uint32_t active:32;	// 0
    };
    uint32_t raw;
};
union nvic_icer0_t {
    struct {
        uint32_t clrena:32;	// 0
    };
    uint32_t raw;
};
union nvic_icer1_t {
    struct {
        uint32_t clrena:32;	// 0
    };
    uint32_t raw;
};
union nvic_icer2_t {
    struct {
        uint32_t clrena:32;	// 0
    };
    uint32_t raw;
};
union nvic_icer3_t {
    struct {
        uint32_t clrena:32;	// 0
    };
    uint32_t raw;
};
union nvic_icpr0_t {
    struct {
        uint32_t clrpend:32;	// 0
    };
    uint32_t raw;
};
union nvic_icpr1_t {
    struct {
        uint32_t clrpend:32;	// 0
    };
    uint32_t raw;
};
union nvic_icpr2_t {
    struct {
        uint32_t clrpend:32;	// 0
    };
    uint32_t raw;
};
union nvic_icpr3_t {
    struct {
        uint32_t clrpend:32;	// 0
    };
    uint32_t raw;
};
union nvic_ipr0_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr1_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr10_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr11_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr12_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr13_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr14_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr15_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr16_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr17_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr18_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr19_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr2_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr20_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr21_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr22_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr23_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr24_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr25_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr26_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr27_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr3_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr4_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr5_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr6_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr7_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr8_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_ipr9_t {
    struct {
        uint32_t pri_0:8;	// 0
        uint32_t pri_1:8;	// 8
        uint32_t pri_2:8;	// 16
        uint32_t pri_3:8;	// 24
    };
    uint32_t raw;
};
union nvic_iser0_t {
    struct {
        uint32_t setena:32;	// 0
    };
    uint32_t raw;
};
union nvic_iser1_t {
    struct {
        uint32_t setena:32;	// 0
    };
    uint32_t raw;
};
union nvic_iser2_t {
    struct {
        uint32_t setena:32;	// 0
    };
    uint32_t raw;
};
union nvic_iser3_t {
    struct {
        uint32_t setena:32;	// 0
    };
    uint32_t raw;
};
union nvic_ispr0_t {
    struct {
        uint32_t setpend:32;	// 0
    };
    uint32_t raw;
};
union nvic_ispr1_t {
    struct {
        uint32_t setpend:32;	// 0
    };
    uint32_t raw;
};
union nvic_ispr2_t {
    struct {
        uint32_t setpend:32;	// 0
    };
    uint32_t raw;
};
union nvic_ispr3_t {
    struct {
        uint32_t setpend:32;	// 0
    };
    uint32_t raw;
};
union shpr1_t {
    struct {
        uint32_t pri_4:8;	// 0
        uint32_t pri_5:8;	// 8
        uint32_t pri_6:8;	// 16
    };
    uint32_t raw;
};
union shpr2_t {
    struct {
        uint32_t :24;
        uint32_t pri_11:8;	// 24
    };
    uint32_t raw;
};
union shpr3_t {
    struct {
        uint32_t :16;
        uint32_t pri_14:8;	// 16
        uint32_t pri_15:8;	// 24
    };
    uint32_t raw;
};
union syst_calib_t {
    struct {
        uint32_t tenms:24;	// 0
        uint32_t :6;
        uint32_t skew:1;	// 30
        uint32_t noref:1;	// 31
    };
    uint32_t raw;
};
union syst_csr_t {
    struct {
        uint32_t enable:1;	// 0
        uint32_t tickint:1;	// 1
        uint32_t clksource:1;	// 2
        uint32_t :13;
        uint32_t countflag:1;	// 16
    };
    uint32_t raw;
};
union syst_cvr_t {
    struct {
        uint32_t current:24;	// 0
    };
    uint32_t raw;
};
union syst_rvr_t {
    struct {
        uint32_t reload:24;	// 0
    };
    uint32_t raw;
};
}
namespace rtc_ns {
union id_t {
    struct {
        uint32_t mod_rev:8;	// 0
        uint32_t mod_type:8;	// 8
        uint32_t mod_number:16;	// 16
    };
    uint32_t raw;
};
}
namespace scu_clk_ns {
union ccuclkcr_t {
    struct {
        uint32_t ccudiv:1;	// 0
    };
    uint32_t raw;
};
union cgatclr0_t {
    struct {
        uint32_t vadc:1;	// 0
        uint32_t dsd:1;	// 1
        uint32_t ccu40:1;	// 2
        uint32_t ccu41:1;	// 3
        uint32_t ccu42:1;	// 4
        uint32_t :2;
        uint32_t ccu80:1;	// 7
        uint32_t ccu81:1;	// 8
        uint32_t posif0:1;	// 9
        uint32_t posif1:1;	// 10
        uint32_t usic0:1;	// 11
        uint32_t :4;
        uint32_t eru1:1;	// 16
        uint32_t :6;
        uint32_t hrpwm0:1;	// 23
    };
    uint32_t raw;
};
union cgatclr1_t {
    struct {
        uint32_t ccu43:1;	// 0
        uint32_t :2;
        uint32_t ledtscu0:1;	// 3
        uint32_t mcan0:1;	// 4
        uint32_t dac:1;	// 5
        uint32_t :1;
        uint32_t usic1:1;	// 7
        uint32_t :1;
        uint32_t pports:1;	// 9
    };
    uint32_t raw;
};
union cgatclr2_t {
    struct {
        uint32_t :1;
        uint32_t wdt:1;	// 1
        uint32_t eth0:1;	// 2
        uint32_t :1;
        uint32_t dma0:1;	// 4
        uint32_t :1;
        uint32_t fce:1;	// 6
        uint32_t usb:1;	// 7
    };
    uint32_t raw;
};
union cgatset0_t {
    struct {
        uint32_t vadc:1;	// 0
        uint32_t dsd:1;	// 1
        uint32_t ccu40:1;	// 2
        uint32_t ccu41:1;	// 3
        uint32_t ccu42:1;	// 4
        uint32_t :2;
        uint32_t ccu80:1;	// 7
        uint32_t ccu81:1;	// 8
        uint32_t posif0:1;	// 9
        uint32_t posif1:1;	// 10
        uint32_t usic0:1;	// 11
        uint32_t :4;
        uint32_t eru1:1;	// 16
        uint32_t :6;
        uint32_t hrpwm0:1;	// 23
    };
    uint32_t raw;
};
union cgatset1_t {
    struct {
        uint32_t ccu43:1;	// 0
        uint32_t :2;
        uint32_t ledtscu0:1;	// 3
        uint32_t mcan0:1;	// 4
        uint32_t dac:1;	// 5
        uint32_t :1;
        uint32_t usic1:1;	// 7
        uint32_t :1;
        uint32_t pports:1;	// 9
    };
    uint32_t raw;
};
union cgatset2_t {
    struct {
        uint32_t :1;
        uint32_t wdt:1;	// 1
        uint32_t eth0:1;	// 2
        uint32_t :1;
        uint32_t dma0:1;	// 4
        uint32_t :1;
        uint32_t fce:1;	// 6
        uint32_t usb:1;	// 7
    };
    uint32_t raw;
};
union cgatstat0_t {
    struct {
        uint32_t vadc:1;	// 0
        uint32_t dsd:1;	// 1
        uint32_t ccu40:1;	// 2
        uint32_t ccu41:1;	// 3
        uint32_t ccu42:1;	// 4
        uint32_t :2;
        uint32_t ccu80:1;	// 7
        uint32_t ccu81:1;	// 8
        uint32_t posif0:1;	// 9
        uint32_t posif1:1;	// 10
        uint32_t usic0:1;	// 11
        uint32_t :4;
        uint32_t eru1:1;	// 16
        uint32_t :6;
        uint32_t hrpwm0:1;	// 23
    };
    uint32_t raw;
};
union cgatstat1_t {
    struct {
        uint32_t ccu43:1;	// 0
        uint32_t :2;
        uint32_t ledtscu0:1;	// 3
        uint32_t mcan0:1;	// 4
        uint32_t dac:1;	// 5
        uint32_t :1;
        uint32_t usic1:1;	// 7
        uint32_t :1;
        uint32_t pports:1;	// 9
    };
    uint32_t raw;
};
union cgatstat2_t {
    struct {
        uint32_t :1;
        uint32_t wdt:1;	// 1
        uint32_t eth0:1;	// 2
        uint32_t :1;
        uint32_t dma0:1;	// 4
        uint32_t :1;
        uint32_t fce:1;	// 6
        uint32_t usb:1;	// 7
    };
    uint32_t raw;
};
union clkclr_t {
    struct {
        uint32_t usbcdi:1;	// 0
        uint32_t :1;
        uint32_t eth0cdi:1;	// 2
        uint32_t :1;
        uint32_t ccucdi:1;	// 4
        uint32_t wdtcdi:1;	// 5
    };
    uint32_t raw;
};
union clkset_t {
    struct {
        uint32_t usbcen:1;	// 0
        uint32_t :1;
        uint32_t eth0cen:1;	// 2
        uint32_t :1;
        uint32_t ccucen:1;	// 4
        uint32_t wdtcen:1;	// 5
    };
    uint32_t raw;
};
union clkstat_t {
    struct {
        uint32_t usbcst:1;	// 0
        uint32_t :1;
        uint32_t eth0cst:1;	// 2
        uint32_t :1;
        uint32_t ccucst:1;	// 4
        uint32_t wdtcst:1;	// 5
    };
    uint32_t raw;
};
union cpuclkcr_t {
    struct {
        uint32_t cpudiv:1;	// 0
    };
    uint32_t raw;
};
union dsleepcr_t {
    struct {
        uint32_t syssel:1;	// 0
        uint32_t :10;
        uint32_t fpdn:1;	// 11
        uint32_t pllpdn:1;	// 12
        uint32_t vcopdn:1;	// 13
        uint32_t :2;
        uint32_t usbcr:1;	// 16
        uint32_t :1;
        uint32_t eth0cr:1;	// 18
        uint32_t :1;
        uint32_t ccucr:1;	// 20
        uint32_t wdtcr:1;	// 21
    };
    uint32_t raw;
};
union extclkcr_t {
    struct {
        uint32_t ecksel:3;	// 0
        uint32_t :13;
        uint32_t eckdiv:9;	// 16
    };
    uint32_t raw;
};
union mlinkclkcr_t {
    struct {
        uint32_t sysdiv:8;	// 0
        uint32_t syssel:1;	// 8
        uint32_t :1;
        uint32_t cpudiv:1;	// 10
        uint32_t :1;
        uint32_t pbdiv:1;	// 12
        uint32_t :1;
        uint32_t ccudiv:1;	// 14
        uint32_t :1;
        uint32_t wdtdiv:8;	// 16
        uint32_t wdtsel:2;	// 24
    };
    uint32_t raw;
};
union pbclkcr_t {
    struct {
        uint32_t pbdiv:1;	// 0
    };
    uint32_t raw;
};
union sleepcr_t {
    struct {
        uint32_t syssel:1;	// 0
        uint32_t :15;
        uint32_t usbcr:1;	// 16
        uint32_t :1;
        uint32_t eth0cr:1;	// 18
        uint32_t :1;
        uint32_t ccucr:1;	// 20
        uint32_t wdtcr:1;	// 21
    };
    uint32_t raw;
};
union sysclkcr_t {
    struct {
        uint32_t sysdiv:8;	// 0
        uint32_t :8;
        uint32_t syssel:1;	// 16
    };
    uint32_t raw;
};
union usbclkcr_t {
    struct {
        uint32_t usbdiv:3;	// 0
        uint32_t :13;
        uint32_t usbsel:1;	// 16
    };
    uint32_t raw;
};
union wdtclkcr_t {
    struct {
        uint32_t wdtdiv:8;	// 0
        uint32_t :8;
        uint32_t wdtsel:2;	// 16
    };
    uint32_t raw;
};
}
namespace scu_general_ns {
union ccucon_t {
    struct {
        uint32_t gsc40:1;	// 0
        uint32_t gsc41:1;	// 1
        uint32_t gsc42:1;	// 2
        uint32_t gsc43:1;	// 3
        uint32_t :4;
        uint32_t gsc80:1;	// 8
        uint32_t gsc81:1;	// 9
        uint32_t :14;
        uint32_t gshr0:1;	// 24
    };
    uint32_t raw;
};
union dtempalarm_t {
    struct {
        uint32_t underfl:1;	// 0
        uint32_t :15;
        uint32_t overfl:1;	// 16
    };
    uint32_t raw;
};
union dtemplim_t {
    struct {
        uint32_t lower:10;	// 0
        uint32_t :6;
        uint32_t upper:10;	// 16
    };
    uint32_t raw;
};
union dtscon_t {
    struct {
        uint32_t pwd:1;	// 0
        uint32_t start:1;	// 1
        uint32_t :2;
        uint32_t offset:7;	// 4
        uint32_t gain:6;	// 11
        uint32_t reftrim:3;	// 17
        uint32_t bgtrim:4;	// 20
    };
    uint32_t raw;
};
union dtsstat_t {
    struct {
        uint32_t result:10;	// 0
        uint32_t :4;
        uint32_t rdy:1;	// 14
        uint32_t busy:1;	// 15
    };
    uint32_t raw;
};
union gorcen_t {
    struct {
        uint32_t :6;
        uint32_t enorc6:1;	// 6
        uint32_t enorc7:1;	// 7
    };
    uint32_t raw;
};
union gpr_t {
    struct {
        uint32_t dat:32;	// 0
    };
    uint32_t raw;
};
union id_t {
    struct {
        uint32_t mod_rev:8;	// 0
        uint32_t mod_type:8;	// 8
        uint32_t mod_number:16;	// 16
    };
    uint32_t raw;
};
union idchip_t {
    struct {
        uint32_t idchip:32;	// 0
    };
    uint32_t raw;
};
union idmanuf_t {
    struct {
        uint32_t dept:5;	// 0
        uint32_t manuf:11;	// 5
    };
    uint32_t raw;
};
union mirrallreq_t {
    struct {
        uint32_t req:1;	// 0
    };
    uint32_t raw;
};
union mirrallstat_t {
    struct {
        uint32_t busy:1;	// 0
    };
    uint32_t raw;
};
union mirrsts_t {
    struct {
        uint32_t :1;
        uint32_t hdclr:1;	// 1
        uint32_t hdset:1;	// 2
        uint32_t hdcr:1;	// 3
        uint32_t :1;
        uint32_t oscsictrl:1;	// 5
        uint32_t osculstat:1;	// 6
        uint32_t osculctrl:1;	// 7
        uint32_t rtc_ctr:1;	// 8
        uint32_t rtc_atim0:1;	// 9
        uint32_t rtc_atim1:1;	// 10
        uint32_t rtc_tim0:1;	// 11
        uint32_t rtc_tim1:1;	// 12
        uint32_t rmx:1;	// 13
        uint32_t rtc_msksr:1;	// 14
        uint32_t rtc_clrsr:1;	// 15
        uint32_t lpacconf:1;	// 16
        uint32_t lpacth0:1;	// 17
        uint32_t lpacth1:1;	// 18
        uint32_t :1;
        uint32_t lpacclr:1;	// 20
        uint32_t lpacset:1;	// 21
        uint32_t :1;
        uint32_t hintclr:1;	// 23
        uint32_t hintset:1;	// 24
    };
    uint32_t raw;
};
union rmacr_t {
    struct {
        uint32_t rdwr:1;	// 0
        uint32_t :15;
        uint32_t addr:4;	// 16
    };
    uint32_t raw;
};
union rmdata_t {
    struct {
        uint32_t data:32;	// 0
    };
    uint32_t raw;
};
union stcon_t {
    struct {
        uint32_t hwcon:2;	// 0
        uint32_t :6;
        uint32_t swcon:4;	// 8
    };
    uint32_t raw;
};
}
namespace scu_hibernate_ns {
union hdclr_t {
    struct {
        uint32_t epev:1;	// 0
        uint32_t enev:1;	// 1
        uint32_t rtcev:1;	// 2
        uint32_t ulpwdg:1;	// 3
        uint32_t :4;
        uint32_t vbatpev:1;	// 8
        uint32_t vbatnev:1;	// 9
        uint32_t ahibio0pev:1;	// 10
        uint32_t ahibio0nev:1;	// 11
        uint32_t ahibio1pev:1;	// 12
        uint32_t ahibio1nev:1;	// 13
    };
    uint32_t raw;
};
union hdcr_t {
    struct {
        uint32_t wkpep:1;	// 0
        uint32_t wkpen:1;	// 1
        uint32_t rtce:1;	// 2
        uint32_t ulpwdgen:1;	// 3
        uint32_t hib:1;	// 4
        uint32_t xtalgpi1sel:1;	// 5
        uint32_t rcs:1;	// 6
        uint32_t stdbysel:1;	// 7
        uint32_t wkupsel:1;	// 8
        uint32_t :1;
        uint32_t gpi0sel:1;	// 10
        uint32_t gpi1sel:1;	// 11
        uint32_t hibio0pol:1;	// 12
        uint32_t hibio1pol:1;	// 13
        uint32_t adig0sel:1;	// 14
        uint32_t adig1sel:1;	// 15
        uint32_t hibio0sel:4;	// 16
        uint32_t hibio1sel:4;	// 20
        uint32_t vbatlo:1;	// 24
        uint32_t vbathi:1;	// 25
        uint32_t ahibio0lo:1;	// 26
        uint32_t ahibio0hi:1;	// 27
        uint32_t ahibio1lo:1;	// 28
        uint32_t ahibio1hi:1;	// 29
    };
    uint32_t raw;
};
union hdset_t {
    struct {
        uint32_t epev:1;	// 0
        uint32_t enev:1;	// 1
        uint32_t rtcev:1;	// 2
        uint32_t ulpwdg:1;	// 3
        uint32_t :4;
        uint32_t vbatpev:1;	// 8
        uint32_t vbatnev:1;	// 9
        uint32_t ahibio0pev:1;	// 10
        uint32_t ahibio0nev:1;	// 11
        uint32_t ahibio1pev:1;	// 12
        uint32_t ahibio1nev:1;	// 13
    };
    uint32_t raw;
};
union hdstat_t {
    struct {
        uint32_t epev:1;	// 0
        uint32_t enev:1;	// 1
        uint32_t rtcev:1;	// 2
        uint32_t ulpwdg:1;	// 3
        uint32_t hibnout:1;	// 4
        uint32_t :3;
        uint32_t vbatpev:1;	// 8
        uint32_t vbatnev:1;	// 9
        uint32_t ahibio0pev:1;	// 10
        uint32_t ahibio0nev:1;	// 11
        uint32_t ahibio1pev:1;	// 12
        uint32_t ahibio1nev:1;	// 13
    };
    uint32_t raw;
};
union hintclr_t {
    struct {
        uint32_t hibnint:1;	// 0
        uint32_t :1;
        uint32_t flashoff:1;	// 2
        uint32_t flashpd:1;	// 3
        uint32_t poffd:1;	// 4
        uint32_t :11;
        uint32_t ppodel:2;	// 16
        uint32_t :2;
        uint32_t poffh:1;	// 20
    };
    uint32_t raw;
};
union hintset_t {
    struct {
        uint32_t hibnint:1;	// 0
        uint32_t vcoreoff:1;	// 1
        uint32_t flashoff:1;	// 2
        uint32_t flashpd:1;	// 3
        uint32_t poffd:1;	// 4
        uint32_t :11;
        uint32_t ppodel:2;	// 16
        uint32_t :2;
        uint32_t poffh:1;	// 20
    };
    uint32_t raw;
};
union hintst_t {
    struct {
        uint32_t hibnint:1;	// 0
        uint32_t :1;
        uint32_t flashoff:1;	// 2
        uint32_t flashpd:1;	// 3
        uint32_t poffd:1;	// 4
        uint32_t :11;
        uint32_t ppodel:2;	// 16
        uint32_t :2;
        uint32_t poffh:1;	// 20
    };
    uint32_t raw;
};
union lpacclr_t {
    struct {
        uint32_t vbatscmp:1;	// 0
        uint32_t ahibio0scmp:1;	// 1
        uint32_t ahibio1scmp:1;	// 2
        uint32_t :13;
        uint32_t vbatval:1;	// 16
        uint32_t ahibio0val:1;	// 17
        uint32_t ahibio1val:1;	// 18
    };
    uint32_t raw;
};
union lpacconf_t {
    struct {
        uint32_t cmpen:3;	// 0
        uint32_t :1;
        uint32_t trigsel:3;	// 4
        uint32_t :5;
        uint32_t convdel:1;	// 12
        uint32_t :3;
        uint32_t intervcnt:12;	// 16
        uint32_t settlecnt:4;	// 28
    };
    uint32_t raw;
};
union lpacset_t {
    struct {
        uint32_t vbatscmp:1;	// 0
        uint32_t ahibio0scmp:1;	// 1
        uint32_t ahibio1scmp:1;	// 2
        uint32_t :13;
        uint32_t vbatval:1;	// 16
        uint32_t ahibio0val:1;	// 17
        uint32_t ahibio1val:1;	// 18
    };
    uint32_t raw;
};
union lpacst_t {
    struct {
        uint32_t vbatscmp:1;	// 0
        uint32_t ahibio0scmp:1;	// 1
        uint32_t ahibio1scmp:1;	// 2
        uint32_t :13;
        uint32_t vbatval:1;	// 16
        uint32_t ahibio0val:1;	// 17
        uint32_t ahibio1val:1;	// 18
    };
    uint32_t raw;
};
union lpacth0_t {
    struct {
        uint32_t vbatlo:6;	// 0
        uint32_t :2;
        uint32_t vbathi:6;	// 8
    };
    uint32_t raw;
};
union lpacth1_t {
    struct {
        uint32_t ahibio0lo:6;	// 0
        uint32_t :2;
        uint32_t ahibio0hi:6;	// 8
        uint32_t :2;
        uint32_t ahibio1lo:6;	// 16
        uint32_t :2;
        uint32_t ahibio1hi:6;	// 24
    };
    uint32_t raw;
};
union oscsictrl_t {
    struct {
        uint32_t pwd:1;	// 0
    };
    uint32_t raw;
};
union osculctrl_t {
    struct {
        uint32_t x1den:1;	// 0
        uint32_t :3;
        uint32_t mode:2;	// 4
    };
    uint32_t raw;
};
union osculstat_t {
    struct {
        uint32_t x1d:1;	// 0
    };
    uint32_t raw;
};
}
namespace scu_interrupt_ns {
union nmireqen_t {
    struct {
        uint32_t prwarn:1;	// 0
        uint32_t pi:1;	// 1
        uint32_t ai:1;	// 2
        uint32_t :13;
        uint32_t eru00:1;	// 16
        uint32_t eru01:1;	// 17
        uint32_t eru02:1;	// 18
        uint32_t eru03:1;	// 19
    };
    uint32_t raw;
};
union srclr_t {
    struct {
        uint32_t prwarn:1;	// 0
        uint32_t pi:1;	// 1
        uint32_t ai:1;	// 2
        uint32_t dlrovr:1;	// 3
        uint32_t :2;
        uint32_t lpaccr:1;	// 6
        uint32_t lpacth0:1;	// 7
        uint32_t lpacth1:1;	// 8
        uint32_t lpacst:1;	// 9
        uint32_t lpacclr:1;	// 10
        uint32_t lpacset:1;	// 11
        uint32_t hintst:1;	// 12
        uint32_t hintclr:1;	// 13
        uint32_t hintset:1;	// 14
        uint32_t :1;
        uint32_t hdstat:1;	// 16
        uint32_t hdclr:1;	// 17
        uint32_t hdset:1;	// 18
        uint32_t hdcr:1;	// 19
        uint32_t :1;
        uint32_t oscsictrl:1;	// 21
        uint32_t osculstat:1;	// 22
        uint32_t osculctrl:1;	// 23
        uint32_t rtc_ctr:1;	// 24
        uint32_t rtc_atim0:1;	// 25
        uint32_t rtc_atim1:1;	// 26
        uint32_t rtc_tim0:1;	// 27
        uint32_t rtc_tim1:1;	// 28
        uint32_t rmx:1;	// 29
    };
    uint32_t raw;
};
union srmsk_t {
    struct {
        uint32_t prwarn:1;	// 0
        uint32_t pi:1;	// 1
        uint32_t ai:1;	// 2
        uint32_t dlrovr:1;	// 3
        uint32_t :2;
        uint32_t lpaccr:1;	// 6
        uint32_t lpacth0:1;	// 7
        uint32_t lpacth1:1;	// 8
        uint32_t lpacst:1;	// 9
        uint32_t lpacclr:1;	// 10
        uint32_t lpacset:1;	// 11
        uint32_t hintst:1;	// 12
        uint32_t hintclr:1;	// 13
        uint32_t hintset:1;	// 14
        uint32_t :1;
        uint32_t hdstat:1;	// 16
        uint32_t hdclr:1;	// 17
        uint32_t hdset:1;	// 18
        uint32_t hdcr:1;	// 19
        uint32_t :1;
        uint32_t oscsictrl:1;	// 21
        uint32_t osculstat:1;	// 22
        uint32_t osculctrl:1;	// 23
        uint32_t rtc_ctr:1;	// 24
        uint32_t rtc_atim0:1;	// 25
        uint32_t rtc_atim1:1;	// 26
        uint32_t rtc_tim0:1;	// 27
        uint32_t rtc_tim1:1;	// 28
        uint32_t rmx:1;	// 29
    };
    uint32_t raw;
};
union srraw_t {
    struct {
        uint32_t prwarn:1;	// 0
        uint32_t pi:1;	// 1
        uint32_t ai:1;	// 2
        uint32_t dlrovr:1;	// 3
        uint32_t :2;
        uint32_t lpaccr:1;	// 6
        uint32_t lpacth0:1;	// 7
        uint32_t lpacth1:1;	// 8
        uint32_t lpacst:1;	// 9
        uint32_t lpacclr:1;	// 10
        uint32_t lpacset:1;	// 11
        uint32_t hintst:1;	// 12
        uint32_t hintclr:1;	// 13
        uint32_t hintset:1;	// 14
        uint32_t :1;
        uint32_t hdstat:1;	// 16
        uint32_t hdclr:1;	// 17
        uint32_t hdset:1;	// 18
        uint32_t hdcr:1;	// 19
        uint32_t :1;
        uint32_t oscsictrl:1;	// 21
        uint32_t osculstat:1;	// 22
        uint32_t osculctrl:1;	// 23
        uint32_t rtc_ctr:1;	// 24
        uint32_t rtc_atim0:1;	// 25
        uint32_t rtc_atim1:1;	// 26
        uint32_t rtc_tim0:1;	// 27
        uint32_t rtc_tim1:1;	// 28
        uint32_t rmx:1;	// 29
    };
    uint32_t raw;
};
union srset_t {
    struct {
        uint32_t prwarn:1;	// 0
        uint32_t pi:1;	// 1
        uint32_t ai:1;	// 2
        uint32_t dlrovr:1;	// 3
        uint32_t :2;
        uint32_t lpaccr:1;	// 6
        uint32_t lpacth0:1;	// 7
        uint32_t lpacth1:1;	// 8
        uint32_t lpacst:1;	// 9
        uint32_t lpacclr:1;	// 10
        uint32_t lpacset:1;	// 11
        uint32_t hintst:1;	// 12
        uint32_t hintclr:1;	// 13
        uint32_t hintset:1;	// 14
        uint32_t :1;
        uint32_t hdstat:1;	// 16
        uint32_t hdcrclr:1;	// 17
        uint32_t hdcrset:1;	// 18
        uint32_t hdcr:1;	// 19
        uint32_t :1;
        uint32_t oscsictrl:1;	// 21
        uint32_t osculstat:1;	// 22
        uint32_t osculctrl:1;	// 23
        uint32_t rtc_ctr:1;	// 24
        uint32_t rtc_atim0:1;	// 25
        uint32_t rtc_atim1:1;	// 26
        uint32_t rtc_tim0:1;	// 27
        uint32_t rtc_tim1:1;	// 28
        uint32_t rmx:1;	// 29
    };
    uint32_t raw;
};
union srstat_t {
    struct {
        uint32_t prwarn:1;	// 0
        uint32_t pi:1;	// 1
        uint32_t ai:1;	// 2
        uint32_t dlrovr:1;	// 3
        uint32_t :2;
        uint32_t lpaccr:1;	// 6
        uint32_t lpacth0:1;	// 7
        uint32_t lpacth1:1;	// 8
        uint32_t lpacst:1;	// 9
        uint32_t lpacclr:1;	// 10
        uint32_t lpacset:1;	// 11
        uint32_t hintst:1;	// 12
        uint32_t hintclr:1;	// 13
        uint32_t hintset:1;	// 14
        uint32_t :1;
        uint32_t hdstat:1;	// 16
        uint32_t hdclr:1;	// 17
        uint32_t hdset:1;	// 18
        uint32_t hdcr:1;	// 19
        uint32_t :1;
        uint32_t oscsictrl:1;	// 21
        uint32_t osculstat:1;	// 22
        uint32_t osculctrl:1;	// 23
        uint32_t rtc_ctr:1;	// 24
        uint32_t rtc_atim0:1;	// 25
        uint32_t rtc_atim1:1;	// 26
        uint32_t rtc_tim0:1;	// 27
        uint32_t rtc_tim1:1;	// 28
        uint32_t rmx:1;	// 29
    };
    uint32_t raw;
};
}
namespace scu_osc_ns {
union clkcalconst_t {
    struct {
        uint32_t calibconst:4;	// 0
    };
    uint32_t raw;
};
union oschpctrl_t {
    struct {
        uint32_t x1den:1;	// 0
        uint32_t shby:1;	// 1
        uint32_t :2;
        uint32_t mode:2;	// 4
        uint32_t :10;
        uint32_t oscval:4;	// 16
    };
    uint32_t raw;
};
union oschpstat_t {
    struct {
        uint32_t x1d:1;	// 0
    };
    uint32_t raw;
};
}
namespace scu_parity_ns {
union mchkcon_t {
    struct {
        uint32_t selps:1;	// 0
        uint32_t selds1:1;	// 1
        uint32_t selds2:1;	// 2
        uint32_t :5;
        uint32_t usic0dra:1;	// 8
        uint32_t usic1dra:1;	// 9
        uint32_t :2;
        uint32_t mcandra:1;	// 12
        uint32_t pprfdra:1;	// 13
        uint32_t :2;
        uint32_t selusb:1;	// 16
        uint32_t seleth0tx:1;	// 17
        uint32_t seleth0rx:1;	// 18
    };
    uint32_t raw;
};
union peen_t {
    struct {
        uint32_t peenps:1;	// 0
        uint32_t peends1:1;	// 1
        uint32_t peends2:1;	// 2
        uint32_t :5;
        uint32_t peenu0:1;	// 8
        uint32_t peenu1:1;	// 9
        uint32_t :2;
        uint32_t peenmc:1;	// 12
        uint32_t peenpprf:1;	// 13
        uint32_t :2;
        uint32_t peenusb:1;	// 16
        uint32_t peeneth0tx:1;	// 17
        uint32_t peeneth0rx:1;	// 18
    };
    uint32_t raw;
};
union peflag_t {
    struct {
        uint32_t pefps:1;	// 0
        uint32_t pefds1:1;	// 1
        uint32_t pefds2:1;	// 2
        uint32_t :5;
        uint32_t pefu0:1;	// 8
        uint32_t pefu1:1;	// 9
        uint32_t :2;
        uint32_t pefmc:1;	// 12
        uint32_t pefpprf:1;	// 13
        uint32_t :2;
        uint32_t peusb:1;	// 16
        uint32_t peeth0tx:1;	// 17
        uint32_t peeth0rx:1;	// 18
    };
    uint32_t raw;
};
union persten_t {
    struct {
        uint32_t rsen:1;	// 0
    };
    uint32_t raw;
};
union pete_t {
    struct {
        uint32_t peteps:1;	// 0
        uint32_t peteds1:1;	// 1
        uint32_t peteds2:1;	// 2
        uint32_t :5;
        uint32_t peteu0:1;	// 8
        uint32_t peteu1:1;	// 9
        uint32_t :2;
        uint32_t petemc:1;	// 12
        uint32_t petepprf:1;	// 13
        uint32_t :2;
        uint32_t peteusb:1;	// 16
        uint32_t peteeth0tx:1;	// 17
        uint32_t peteeth0rx:1;	// 18
    };
    uint32_t raw;
};
union pmtpr_t {
    struct {
        uint32_t pwr:8;	// 0
        uint32_t prd:8;	// 8
    };
    uint32_t raw;
};
union pmtsr_t {
    struct {
        uint32_t mtenps:1;	// 0
        uint32_t mtends1:1;	// 1
        uint32_t mtends2:1;	// 2
        uint32_t :5;
        uint32_t mteu0:1;	// 8
        uint32_t mteu1:1;	// 9
        uint32_t :2;
        uint32_t mtemc:1;	// 12
        uint32_t mtepprf:1;	// 13
        uint32_t :2;
        uint32_t mtusb:1;	// 16
        uint32_t mteth0tx:1;	// 17
        uint32_t mteth0rx:1;	// 18
    };
    uint32_t raw;
};
}
namespace scu_pll_ns {
union clkmxstat_t {
    struct {
        uint32_t sysclkmux:2;	// 0
    };
    uint32_t raw;
};
union pllcon0_t {
    struct {
        uint32_t vcobyp:1;	// 0
        uint32_t vcopwd:1;	// 1
        uint32_t vcotr:1;	// 2
        uint32_t :1;
        uint32_t findis:1;	// 4
        uint32_t :1;
        uint32_t oscdiscdis:1;	// 6
        uint32_t :9;
        uint32_t pllpwd:1;	// 16
        uint32_t oscres:1;	// 17
        uint32_t resld:1;	// 18
        uint32_t aotren:1;	// 19
        uint32_t fotr:1;	// 20
    };
    uint32_t raw;
};
union pllcon1_t {
    struct {
        uint32_t k1div:7;	// 0
        uint32_t :1;
        uint32_t ndiv:7;	// 8
        uint32_t :1;
        uint32_t k2div:7;	// 16
        uint32_t :1;
        uint32_t pdiv:4;	// 24
    };
    uint32_t raw;
};
union pllcon2_t {
    struct {
        uint32_t pinsel:1;	// 0
        uint32_t :7;
        uint32_t k1insel:1;	// 8
    };
    uint32_t raw;
};
union pllstat_t {
    struct {
        uint32_t vcobyst:1;	// 0
        uint32_t pwdstat:1;	// 1
        uint32_t vcolock:1;	// 2
        uint32_t :1;
        uint32_t k1rdy:1;	// 4
        uint32_t k2rdy:1;	// 5
        uint32_t by:1;	// 6
        uint32_t plllv:1;	// 7
        uint32_t pllhv:1;	// 8
        uint32_t pllsp:1;	// 9
    };
    uint32_t raw;
};
union usbpllcon_t {
    struct {
        uint32_t vcobyp:1;	// 0
        uint32_t vcopwd:1;	// 1
        uint32_t vcotr:1;	// 2
        uint32_t :1;
        uint32_t findis:1;	// 4
        uint32_t :1;
        uint32_t oscdiscdis:1;	// 6
        uint32_t :1;
        uint32_t ndiv:7;	// 8
        uint32_t :1;
        uint32_t pllpwd:1;	// 16
        uint32_t :1;
        uint32_t resld:1;	// 18
        uint32_t :5;
        uint32_t pdiv:4;	// 24
    };
    uint32_t raw;
};
union usbpllstat_t {
    struct {
        uint32_t vcobyst:1;	// 0
        uint32_t pwdstat:1;	// 1
        uint32_t vcolock:1;	// 2
        uint32_t :3;
        uint32_t by:1;	// 6
        uint32_t vcolocked:1;	// 7
    };
    uint32_t raw;
};
}
namespace scu_power_ns {
union evrstat_t {
    struct {
        uint32_t :1;
        uint32_t ov13:1;	// 1
    };
    uint32_t raw;
};
union evrvadcstat_t {
    struct {
        uint32_t vadc13v:8;	// 0
        uint32_t vadc33v:8;	// 8
    };
    uint32_t raw;
};
union pwrclr_t {
    struct {
        uint32_t hib:1;	// 0
        uint32_t :15;
        uint32_t usbphypdq:1;	// 16
        uint32_t usbotgen:1;	// 17
        uint32_t usbpuwq:1;	// 18
    };
    uint32_t raw;
};
union pwrmon_t {
    struct {
        uint32_t thrs:8;	// 0
        uint32_t intv:8;	// 8
        uint32_t enb:1;	// 16
    };
    uint32_t raw;
};
union pwrset_t {
    struct {
        uint32_t hib:1;	// 0
        uint32_t :15;
        uint32_t usbphypdq:1;	// 16
        uint32_t usbotgen:1;	// 17
        uint32_t usbpuwq:1;	// 18
    };
    uint32_t raw;
};
union pwrstat_t {
    struct {
        uint32_t hiben:1;	// 0
        uint32_t :15;
        uint32_t usbphypdq:1;	// 16
        uint32_t usbotgen:1;	// 17
        uint32_t usbpuwq:1;	// 18
    };
    uint32_t raw;
};
}
namespace scu_reset_ns {
union prclr0_t {
    struct {
        uint32_t vadcrs:1;	// 0
        uint32_t dsdrs:1;	// 1
        uint32_t ccu40rs:1;	// 2
        uint32_t ccu41rs:1;	// 3
        uint32_t ccu42rs:1;	// 4
        uint32_t :2;
        uint32_t ccu80rs:1;	// 7
        uint32_t ccu81rs:1;	// 8
        uint32_t posif0rs:1;	// 9
        uint32_t posif1rs:1;	// 10
        uint32_t usic0rs:1;	// 11
        uint32_t :4;
        uint32_t eru1rs:1;	// 16
        uint32_t :6;
        uint32_t hrpwm0rs:1;	// 23
    };
    uint32_t raw;
};
union prclr1_t {
    struct {
        uint32_t ccu43rs:1;	// 0
        uint32_t :2;
        uint32_t ledtscu0rs:1;	// 3
        uint32_t mcan0rs:1;	// 4
        uint32_t dacrs:1;	// 5
        uint32_t :1;
        uint32_t usic1rs:1;	// 7
        uint32_t :1;
        uint32_t pportsrs:1;	// 9
    };
    uint32_t raw;
};
union prclr2_t {
    struct {
        uint32_t :1;
        uint32_t wdtrs:1;	// 1
        uint32_t eth0rs:1;	// 2
        uint32_t :1;
        uint32_t dma0rs:1;	// 4
        uint32_t :1;
        uint32_t fcers:1;	// 6
        uint32_t usbrs:1;	// 7
    };
    uint32_t raw;
};
union prset0_t {
    struct {
        uint32_t vadcrs:1;	// 0
        uint32_t dsdrs:1;	// 1
        uint32_t ccu40rs:1;	// 2
        uint32_t ccu41rs:1;	// 3
        uint32_t ccu42rs:1;	// 4
        uint32_t :2;
        uint32_t ccu80rs:1;	// 7
        uint32_t ccu81rs:1;	// 8
        uint32_t posif0rs:1;	// 9
        uint32_t posif1rs:1;	// 10
        uint32_t usic0rs:1;	// 11
        uint32_t :4;
        uint32_t eru1rs:1;	// 16
        uint32_t :6;
        uint32_t hrpwm0rs:1;	// 23
    };
    uint32_t raw;
};
union prset1_t {
    struct {
        uint32_t ccu43rs:1;	// 0
        uint32_t :2;
        uint32_t ledtscu0rs:1;	// 3
        uint32_t mcan0rs:1;	// 4
        uint32_t dacrs:1;	// 5
        uint32_t :1;
        uint32_t usic1rs:1;	// 7
        uint32_t :1;
        uint32_t pportsrs:1;	// 9
    };
    uint32_t raw;
};
union prset2_t {
    struct {
        uint32_t :1;
        uint32_t wdtrs:1;	// 1
        uint32_t eth0rs:1;	// 2
        uint32_t :1;
        uint32_t dma0rs:1;	// 4
        uint32_t :1;
        uint32_t fcers:1;	// 6
        uint32_t usbrs:1;	// 7
    };
    uint32_t raw;
};
union prstat0_t {
    struct {
        uint32_t vadcrs:1;	// 0
        uint32_t dsdrs:1;	// 1
        uint32_t ccu40rs:1;	// 2
        uint32_t ccu41rs:1;	// 3
        uint32_t ccu42rs:1;	// 4
        uint32_t :2;
        uint32_t ccu80rs:1;	// 7
        uint32_t ccu81rs:1;	// 8
        uint32_t posif0rs:1;	// 9
        uint32_t posif1rs:1;	// 10
        uint32_t usic0rs:1;	// 11
        uint32_t :4;
        uint32_t eru1rs:1;	// 16
        uint32_t :6;
        uint32_t hrpwm0rs:1;	// 23
    };
    uint32_t raw;
};
union prstat1_t {
    struct {
        uint32_t ccu43rs:1;	// 0
        uint32_t :2;
        uint32_t ledtscu0rs:1;	// 3
        uint32_t mcan0rs:1;	// 4
        uint32_t dacrs:1;	// 5
        uint32_t :1;
        uint32_t usic1rs:1;	// 7
        uint32_t :1;
        uint32_t pportsrs:1;	// 9
    };
    uint32_t raw;
};
union prstat2_t {
    struct {
        uint32_t :1;
        uint32_t wdtrs:1;	// 1
        uint32_t eth0rs:1;	// 2
        uint32_t :1;
        uint32_t dma0rs:1;	// 4
        uint32_t :1;
        uint32_t fcers:1;	// 6
        uint32_t usbrs:1;	// 7
    };
    uint32_t raw;
};
union rstclr_t {
    struct {
        uint32_t rsclr:1;	// 0
        uint32_t :7;
        uint32_t hibwk:1;	// 8
        uint32_t hibrs:1;	// 9
        uint32_t lcken:1;	// 10
    };
    uint32_t raw;
};
union rstset_t {
    struct {
        uint32_t :8;
        uint32_t hibwk:1;	// 8
        uint32_t hibrs:1;	// 9
        uint32_t lcken:1;	// 10
    };
    uint32_t raw;
};
union rststat_t {
    struct {
        uint32_t rststat:8;	// 0
        uint32_t hibwk:1;	// 8
        uint32_t hibrs:1;	// 9
        uint32_t lcken:1;	// 10
    };
    uint32_t raw;
};
}
namespace scu_trap_ns {
union trapclr_t {
    struct {
        uint32_t soscwdgt:1;	// 0
        uint32_t :1;
        uint32_t svcolckt:1;	// 2
        uint32_t uvcolckt:1;	// 3
        uint32_t pet:1;	// 4
        uint32_t brwnt:1;	// 5
        uint32_t ulpwdgt:1;	// 6
        uint32_t bwerr0t:1;	// 7
        uint32_t bwerr1t:1;	// 8
        uint32_t :3;
        uint32_t temphit:1;	// 12
        uint32_t templot:1;	// 13
    };
    uint32_t raw;
};
union trapdis_t {
    struct {
        uint32_t soscwdgt:1;	// 0
        uint32_t :1;
        uint32_t svcolckt:1;	// 2
        uint32_t uvcolckt:1;	// 3
        uint32_t pet:1;	// 4
        uint32_t brwnt:1;	// 5
        uint32_t ulpwdgt:1;	// 6
        uint32_t bwerr0t:1;	// 7
        uint32_t bwerr1t:1;	// 8
        uint32_t :3;
        uint32_t temphit:1;	// 12
        uint32_t templot:1;	// 13
    };
    uint32_t raw;
};
union trapraw_t {
    struct {
        uint32_t soscwdgt:1;	// 0
        uint32_t :1;
        uint32_t svcolckt:1;	// 2
        uint32_t uvcolckt:1;	// 3
        uint32_t pet:1;	// 4
        uint32_t brwnt:1;	// 5
        uint32_t ulpwdgt:1;	// 6
        uint32_t bwerr0t:1;	// 7
        uint32_t bwerr1t:1;	// 8
        uint32_t :3;
        uint32_t temphit:1;	// 12
        uint32_t templot:1;	// 13
    };
    uint32_t raw;
};
union trapset_t {
    struct {
        uint32_t soscwdgt:1;	// 0
        uint32_t :1;
        uint32_t svcolckt:1;	// 2
        uint32_t uvcolckt:1;	// 3
        uint32_t pet:1;	// 4
        uint32_t brwnt:1;	// 5
        uint32_t ulpwdt:1;	// 6
        uint32_t bwerr0t:1;	// 7
        uint32_t bwerr1t:1;	// 8
        uint32_t :3;
        uint32_t temphit:1;	// 12
        uint32_t templot:1;	// 13
    };
    uint32_t raw;
};
union trapstat_t {
    struct {
        uint32_t soscwdgt:1;	// 0
        uint32_t :1;
        uint32_t svcolckt:1;	// 2
        uint32_t uvcolckt:1;	// 3
        uint32_t pet:1;	// 4
        uint32_t brwnt:1;	// 5
        uint32_t ulpwdgt:1;	// 6
        uint32_t bwerr0t:1;	// 7
        uint32_t bwerr1t:1;	// 8
        uint32_t :3;
        uint32_t temphit:1;	// 12
        uint32_t templot:1;	// 13
    };
    uint32_t raw;
};
}
namespace usic_ns {
union id_t {
    struct {
        uint32_t mod_rev:8;	// 0
        uint32_t mod_type:8;	// 8
        uint32_t mod_number:16;	// 16
    };
    uint32_t raw;
};
}
namespace usic_ch_ns {
union brg_t {
    struct {
        uint32_t clksel:2;	// 0
        uint32_t :1;
        uint32_t tmen:1;	// 3
        uint32_t pppen:1;	// 4
        uint32_t :1;
        uint32_t ctqsel:2;	// 6
        uint32_t pctq:2;	// 8
        uint32_t dctq:5;	// 10
        uint32_t :1;
        uint32_t pdiv:10;	// 16
        uint32_t :2;
        uint32_t sclkosel:1;	// 28
        uint32_t mclkcfg:1;	// 29
        uint32_t sclkcfg:2;	// 30
    };
    uint32_t raw;
};
union byp_t {
    struct {
        uint32_t bdata:16;	// 0
    };
    uint32_t raw;
};
union bypcr_t {
    struct {
        uint32_t bwle:4;	// 0
        uint32_t :4;
        uint32_t bdssm:1;	// 8
        uint32_t :1;
        uint32_t bden:2;	// 10
        uint32_t bdvtr:1;	// 12
        uint32_t bprio:1;	// 13
        uint32_t :1;
        uint32_t bdv:1;	// 15
        uint32_t bselo:5;	// 16
        uint32_t bhpc:3;	// 21
    };
    uint32_t raw;
};
union ccfg_t {
    struct {
        uint32_t ssc:1;	// 0
        uint32_t asc:1;	// 1
        uint32_t iic:1;	// 2
        uint32_t iis:1;	// 3
        uint32_t :2;
        uint32_t rb:1;	// 6
        uint32_t tb:1;	// 7
    };
    uint32_t raw;
};
union ccr_t {
    struct {
        uint32_t mode:4;	// 0
        uint32_t :2;
        uint32_t hpcen:2;	// 6
        uint32_t pm:2;	// 8
        uint32_t rsien:1;	// 10
        uint32_t dlien:1;	// 11
        uint32_t tsien:1;	// 12
        uint32_t tbien:1;	// 13
        uint32_t rien:1;	// 14
        uint32_t aien:1;	// 15
        uint32_t brgien:1;	// 16
    };
    uint32_t raw;
};
union cmtr_t {
    struct {
        uint32_t ctv:10;	// 0
    };
    uint32_t raw;
};
union dx0cr_t {
    struct {
        uint32_t dsel:3;	// 0
        uint32_t :1;
        uint32_t insw:1;	// 4
        uint32_t dfen:1;	// 5
        uint32_t dsen:1;	// 6
        uint32_t :1;
        uint32_t dpol:1;	// 8
        uint32_t sfsel:1;	// 9
        uint32_t cm:2;	// 10
        uint32_t :3;
        uint32_t dxs:1;	// 15
    };
    uint32_t raw;
};
union dx1cr_t {
    struct {
        uint32_t dsel:3;	// 0
        uint32_t dcen:1;	// 3
        uint32_t insw:1;	// 4
        uint32_t dfen:1;	// 5
        uint32_t dsen:1;	// 6
        uint32_t :1;
        uint32_t dpol:1;	// 8
        uint32_t sfsel:1;	// 9
        uint32_t cm:2;	// 10
        uint32_t :3;
        uint32_t dxs:1;	// 15
    };
    uint32_t raw;
};
union dx2cr_t {
    struct {
        uint32_t dsel:3;	// 0
        uint32_t :1;
        uint32_t insw:1;	// 4
        uint32_t dfen:1;	// 5
        uint32_t dsen:1;	// 6
        uint32_t :1;
        uint32_t dpol:1;	// 8
        uint32_t sfsel:1;	// 9
        uint32_t cm:2;	// 10
        uint32_t :3;
        uint32_t dxs:1;	// 15
    };
    uint32_t raw;
};
union dx3cr_t {
    struct {
        uint32_t dsel:3;	// 0
        uint32_t :1;
        uint32_t insw:1;	// 4
        uint32_t dfen:1;	// 5
        uint32_t dsen:1;	// 6
        uint32_t :1;
        uint32_t dpol:1;	// 8
        uint32_t sfsel:1;	// 9
        uint32_t cm:2;	// 10
        uint32_t :3;
        uint32_t dxs:1;	// 15
    };
    uint32_t raw;
};
union dx4cr_t {
    struct {
        uint32_t dsel:3;	// 0
        uint32_t :1;
        uint32_t insw:1;	// 4
        uint32_t dfen:1;	// 5
        uint32_t dsen:1;	// 6
        uint32_t :1;
        uint32_t dpol:1;	// 8
        uint32_t sfsel:1;	// 9
        uint32_t cm:2;	// 10
        uint32_t :3;
        uint32_t dxs:1;	// 15
    };
    uint32_t raw;
};
union dx5cr_t {
    struct {
        uint32_t dsel:3;	// 0
        uint32_t :1;
        uint32_t insw:1;	// 4
        uint32_t dfen:1;	// 5
        uint32_t dsen:1;	// 6
        uint32_t :1;
        uint32_t dpol:1;	// 8
        uint32_t sfsel:1;	// 9
        uint32_t cm:2;	// 10
        uint32_t :3;
        uint32_t dxs:1;	// 15
    };
    uint32_t raw;
};
union fdr_t {
    struct {
        uint32_t step:10;	// 0
        uint32_t :4;
        uint32_t dm:2;	// 14
        uint32_t result:10;	// 16
    };
    uint32_t raw;
};
union fmr_t {
    struct {
        uint32_t mtdv:2;	// 0
        uint32_t :2;
        uint32_t atvc:1;	// 4
        uint32_t :9;
        uint32_t crdv0:1;	// 14
        uint32_t crdv1:1;	// 15
        uint32_t sio0:1;	// 16
        uint32_t sio1:1;	// 17
        uint32_t sio2:1;	// 18
        uint32_t sio3:1;	// 19
        uint32_t sio4:1;	// 20
        uint32_t sio5:1;	// 21
    };
    uint32_t raw;
};
union in_t {
    struct {
        uint32_t tdata:16;	// 0
    };
    uint32_t raw;
};
union inpr_t {
    struct {
        uint32_t tsinp:3;	// 0
        uint32_t :1;
        uint32_t tbinp:3;	// 4
        uint32_t :1;
        uint32_t rinp:3;	// 8
        uint32_t :1;
        uint32_t ainp:3;	// 12
        uint32_t :1;
        uint32_t pinp:3;	// 16
    };
    uint32_t raw;
};
union kscfg_t {
    struct {
        uint32_t moden:1;	// 0
        uint32_t bpmoden:1;	// 1
        uint32_t :2;
        uint32_t nomcfg:2;	// 4
        uint32_t :1;
        uint32_t bpnom:1;	// 7
        uint32_t sumcfg:2;	// 8
        uint32_t :1;
        uint32_t bpsum:1;	// 11
    };
    uint32_t raw;
};
union outdr_t {
    struct {
        uint32_t dsr:16;	// 0
        uint32_t rci:5;	// 16
    };
    uint32_t raw;
};
union outr_t {
    struct {
        uint32_t dsr:16;	// 0
        uint32_t rci:5;	// 16
    };
    uint32_t raw;
};
union pcr_t {
    struct {
        uint32_t ctr0:1;	// 0
        uint32_t ctr1:1;	// 1
        uint32_t ctr2:1;	// 2
        uint32_t ctr3:1;	// 3
        uint32_t ctr4:1;	// 4
        uint32_t ctr5:1;	// 5
        uint32_t ctr6:1;	// 6
        uint32_t ctr7:1;	// 7
        uint32_t ctr8:1;	// 8
        uint32_t ctr9:1;	// 9
        uint32_t ctr10:1;	// 10
        uint32_t ctr11:1;	// 11
        uint32_t ctr12:1;	// 12
        uint32_t ctr13:1;	// 13
        uint32_t ctr14:1;	// 14
        uint32_t ctr15:1;	// 15
        uint32_t ctr16:1;	// 16
        uint32_t ctr17:1;	// 17
        uint32_t ctr18:1;	// 18
        uint32_t ctr19:1;	// 19
        uint32_t ctr20:1;	// 20
        uint32_t ctr21:1;	// 21
        uint32_t ctr22:1;	// 22
        uint32_t ctr23:1;	// 23
        uint32_t ctr24:1;	// 24
        uint32_t ctr25:1;	// 25
        uint32_t ctr26:1;	// 26
        uint32_t ctr27:1;	// 27
        uint32_t ctr28:1;	// 28
        uint32_t ctr29:1;	// 29
        uint32_t ctr30:1;	// 30
        uint32_t ctr31:1;	// 31
    };
    uint32_t raw;
};
union pscr_t {
    struct {
        uint32_t cst0:1;	// 0
        uint32_t cst1:1;	// 1
        uint32_t cst2:1;	// 2
        uint32_t cst3:1;	// 3
        uint32_t cst4:1;	// 4
        uint32_t cst5:1;	// 5
        uint32_t cst6:1;	// 6
        uint32_t cst7:1;	// 7
        uint32_t cst8:1;	// 8
        uint32_t cst9:1;	// 9
        uint32_t crsif:1;	// 10
        uint32_t cdlif:1;	// 11
        uint32_t ctsif:1;	// 12
        uint32_t ctbif:1;	// 13
        uint32_t crif:1;	// 14
        uint32_t caif:1;	// 15
        uint32_t cbrgif:1;	// 16
    };
    uint32_t raw;
};
union psr_t {
    struct {
        uint32_t st0:1;	// 0
        uint32_t st1:1;	// 1
        uint32_t st2:1;	// 2
        uint32_t st3:1;	// 3
        uint32_t st4:1;	// 4
        uint32_t st5:1;	// 5
        uint32_t st6:1;	// 6
        uint32_t st7:1;	// 7
        uint32_t st8:1;	// 8
        uint32_t st9:1;	// 9
        uint32_t rsif:1;	// 10
        uint32_t dlif:1;	// 11
        uint32_t tsif:1;	// 12
        uint32_t tbif:1;	// 13
        uint32_t rif:1;	// 14
        uint32_t aif:1;	// 15
        uint32_t brgif:1;	// 16
    };
    uint32_t raw;
};
union rbctr_t {
    struct {
        uint32_t dptr:6;	// 0
        uint32_t :2;
        uint32_t limit:6;	// 8
        uint32_t srbtm:1;	// 14
        uint32_t srbten:1;	// 15
        uint32_t srbinp:3;	// 16
        uint32_t arbinp:3;	// 19
        uint32_t rcim:2;	// 22
        uint32_t size:3;	// 24
        uint32_t rnm:1;	// 27
        uint32_t lof:1;	// 28
        uint32_t arbien:1;	// 29
        uint32_t srbien:1;	// 30
        uint32_t rberien:1;	// 31
    };
    uint32_t raw;
};
union rbuf_t {
    struct {
        uint32_t dsr:16;	// 0
    };
    uint32_t raw;
};
union rbuf0_t {
    struct {
        uint32_t dsr0:16;	// 0
    };
    uint32_t raw;
};
union rbuf01sr_t {
    struct {
        uint32_t wlen0:4;	// 0
        uint32_t :2;
        uint32_t sof0:1;	// 6
        uint32_t :1;
        uint32_t par0:1;	// 8
        uint32_t perr0:1;	// 9
        uint32_t :3;
        uint32_t rdv00:1;	// 13
        uint32_t rdv01:1;	// 14
        uint32_t ds0:1;	// 15
        uint32_t wlen1:4;	// 16
        uint32_t :2;
        uint32_t sof1:1;	// 22
        uint32_t :1;
        uint32_t par1:1;	// 24
        uint32_t perr1:1;	// 25
        uint32_t :3;
        uint32_t rdv10:1;	// 29
        uint32_t rdv11:1;	// 30
        uint32_t ds1:1;	// 31
    };
    uint32_t raw;
};
union rbuf1_t {
    struct {
        uint32_t dsr1:16;	// 0
    };
    uint32_t raw;
};
union rbufd_t {
    struct {
        uint32_t dsr:16;	// 0
    };
    uint32_t raw;
};
union rbufsr_t {
    struct {
        uint32_t wlen:4;	// 0
        uint32_t :2;
        uint32_t sof:1;	// 6
        uint32_t :1;
        uint32_t par:1;	// 8
        uint32_t perr:1;	// 9
        uint32_t :3;
        uint32_t rdv0:1;	// 13
        uint32_t rdv1:1;	// 14
        uint32_t ds:1;	// 15
    };
    uint32_t raw;
};
union sctr_t {
    struct {
        uint32_t sdir:1;	// 0
        uint32_t pdl:1;	// 1
        uint32_t dsm:2;	// 2
        uint32_t hpcdir:1;	// 4
        uint32_t :1;
        uint32_t docfg:2;	// 6
        uint32_t trm:2;	// 8
        uint32_t :6;
        uint32_t fle:6;	// 16
        uint32_t :2;
        uint32_t wle:4;	// 24
    };
    uint32_t raw;
};
union tbctr_t {
    struct {
        uint32_t dptr:6;	// 0
        uint32_t :2;
        uint32_t limit:6;	// 8
        uint32_t stbtm:1;	// 14
        uint32_t stbten:1;	// 15
        uint32_t stbinp:3;	// 16
        uint32_t atbinp:3;	// 19
        uint32_t :2;
        uint32_t size:3;	// 24
        uint32_t :1;
        uint32_t lof:1;	// 28
        uint32_t :1;
        uint32_t stbien:1;	// 30
        uint32_t tberien:1;	// 31
    };
    uint32_t raw;
};
union tbuf_t {
    struct {
        uint32_t tdata:16;	// 0
    };
    uint32_t raw;
};
union tcsr_t {
    struct {
        uint32_t wlemd:1;	// 0
        uint32_t selmd:1;	// 1
        uint32_t flemd:1;	// 2
        uint32_t wamd:1;	// 3
        uint32_t hpcmd:1;	// 4
        uint32_t sof:1;	// 5
        uint32_t eof:1;	// 6
        uint32_t tdv:1;	// 7
        uint32_t tdssm:1;	// 8
        uint32_t :1;
        uint32_t tden:2;	// 10
        uint32_t tdvtr:1;	// 12
        uint32_t wa:1;	// 13
        uint32_t :10;
        uint32_t tsof:1;	// 24
        uint32_t :1;
        uint32_t tv:1;	// 26
        uint32_t tvc:1;	// 27
        uint32_t te:1;	// 28
    };
    uint32_t raw;
};
union trbptr_t {
    struct {
        uint32_t tdiptr:6;	// 0
        uint32_t :2;
        uint32_t tdoptr:6;	// 8
        uint32_t :2;
        uint32_t rdiptr:6;	// 16
        uint32_t :2;
        uint32_t rdoptr:6;	// 24
    };
    uint32_t raw;
};
union trbscr_t {
    struct {
        uint32_t csrbi:1;	// 0
        uint32_t crberi:1;	// 1
        uint32_t carbi:1;	// 2
        uint32_t :5;
        uint32_t cstbi:1;	// 8
        uint32_t ctberi:1;	// 9
        uint32_t cbdv:1;	// 10
        uint32_t :3;
        uint32_t flushrb:1;	// 14
        uint32_t flushtb:1;	// 15
    };
    uint32_t raw;
};
union trbsr_t {
    struct {
        uint32_t srbi:1;	// 0
        uint32_t rberi:1;	// 1
        uint32_t arbi:1;	// 2
        uint32_t rempty:1;	// 3
        uint32_t rfull:1;	// 4
        uint32_t rbus:1;	// 5
        uint32_t srbt:1;	// 6
        uint32_t :1;
        uint32_t stbi:1;	// 8
        uint32_t tberi:1;	// 9
        uint32_t :1;
        uint32_t tempty:1;	// 11
        uint32_t tfull:1;	// 12
        uint32_t tbus:1;	// 13
        uint32_t stbt:1;	// 14
        uint32_t :1;
        uint32_t rbflvl:7;	// 16
        uint32_t :1;
        uint32_t tbflvl:7;	// 24
    };
    uint32_t raw;
};
}
namespace vadc_ns {
union clc_t {
    struct {
	uint32_t disr:1;
	uint32_t diss:1;
	uint32_t :1;
	uint32_t edis:1;
    };
    uint32_t raw;
};
union ocs_d {
    struct {
	uint32_t tgs:2;
	uint32_t tgb:1;
	uint32_t tg_p:1;
	uint32_t :20;
	uint32_t sus:4;
	uint32_t sus_p:1;
	uint32_t sussta:1;
    };
    uint32_t raw;
};
union globcfg_t {
    struct {
	uint32_t diva:5;
	uint32_t :2;
	uint32_t dcmsb:1;
	uint32_t divd:2;
	uint32_t :5;
	uint32_t divwc:1;
	uint32_t dp_cal0:1;
	uint32_t dp_cal1:1;
	uint32_t dp_cal2:1;
	uint32_t dp_cal3:1;
	uint32_t :11;
	uint32_t sucal:1;
    };
    uint32_t raw;
};
union id_t {
    struct {
        uint32_t mod_rev:8;	// 0
        uint32_t mod_type:8;	// 8
        uint32_t mod_number:16;	// 16
    };
    uint32_t raw;
};
union ocs_t {
    struct {
        uint32_t :3;
        uint32_t tg_p:1;	// 3
        uint32_t :24;
        uint32_t sus_p:1;	// 28
    };
    uint32_t raw;
};
}
namespace vadc_g_ns {
union alias_t {
    struct {
        uint32_t alias0:5;	// 0
        uint32_t :3;
        uint32_t alias1:5;	// 8
    };
    uint32_t raw;
};
union arbcfg_t {
    struct {
        uint32_t anonc:2;	// 0
        uint32_t :2;
        uint32_t arbrnd:2;	// 4
        uint32_t :1;
        uint32_t arbm:1;	// 7
        uint32_t :8;
        uint32_t anons:2;	// 16
        uint32_t :10;
        uint32_t cal:1;	// 28
        uint32_t :1;
        uint32_t busy:1;	// 30
        uint32_t sample:1;	// 31
    };
    uint32_t raw;
};
union arbpr_t {
    struct {
        uint32_t prio0:2;	// 0
        uint32_t :1;
        uint32_t csm0:1;	// 3
        uint32_t prio1:2;	// 4
        uint32_t :1;
        uint32_t csm1:1;	// 7
        uint32_t prio2:2;	// 8
        uint32_t :1;
        uint32_t csm2:1;	// 11
        uint32_t :12;
        uint32_t asen0:1;	// 24
        uint32_t asen1:1;	// 25
        uint32_t asen2:1;	// 26
    };
    uint32_t raw;
};
union asctrl_t {
    struct {
        uint32_t srcresreg:4;	// 0
        uint32_t :4;
        uint32_t xtsel:4;	// 8
        uint32_t xtlvl:1;	// 12
        uint32_t xtmode:2;	// 13
        uint32_t xtwc:1;	// 15
        uint32_t gtsel:4;	// 16
        uint32_t gtlvl:1;	// 20
        uint32_t :2;
        uint32_t gtwc:1;	// 23
        uint32_t :4;
        uint32_t tmen:1;	// 28
        uint32_t :2;
        uint32_t tmwc:1;	// 31
    };
    uint32_t raw;
};
union asmr_t {
    struct {
        uint32_t engt:2;	// 0
        uint32_t entr:1;	// 2
        uint32_t ensi:1;	// 3
        uint32_t scan:1;	// 4
        uint32_t ldm:1;	// 5
        uint32_t :1;
        uint32_t reqgt:1;	// 7
        uint32_t clrpnd:1;	// 8
        uint32_t ldev:1;	// 9
        uint32_t :6;
        uint32_t rptdis:1;	// 16
    };
    uint32_t raw;
};
union aspnd_t {
    struct {
        uint32_t chpnd0:1;	// 0
        uint32_t chpnd1:1;	// 1
        uint32_t chpnd2:1;	// 2
        uint32_t chpnd3:1;	// 3
        uint32_t chpnd4:1;	// 4
        uint32_t chpnd5:1;	// 5
        uint32_t chpnd6:1;	// 6
        uint32_t chpnd7:1;	// 7
    };
    uint32_t raw;
};
union assel_t {
    struct {
        uint32_t chsel0:1;	// 0
        uint32_t chsel1:1;	// 1
        uint32_t chsel2:1;	// 2
        uint32_t chsel3:1;	// 3
        uint32_t chsel4:1;	// 4
        uint32_t chsel5:1;	// 5
        uint32_t chsel6:1;	// 6
        uint32_t chsel7:1;	// 7
    };
    uint32_t raw;
};
union bfl_t {
    struct {
        uint32_t bfl0:1;	// 0
        uint32_t bfl1:1;	// 1
        uint32_t bfl2:1;	// 2
        uint32_t bfl3:1;	// 3
        uint32_t :4;
        uint32_t bfa0:1;	// 8
        uint32_t bfa1:1;	// 9
        uint32_t bfa2:1;	// 10
        uint32_t bfa3:1;	// 11
        uint32_t :4;
        uint32_t bfi0:1;	// 16
        uint32_t bfi1:1;	// 17
        uint32_t bfi2:1;	// 18
        uint32_t bfi3:1;	// 19
    };
    uint32_t raw;
};
union bflc_t {
    struct {
        uint32_t bfm0:4;	// 0
        uint32_t bfm1:4;	// 4
        uint32_t bfm2:4;	// 8
        uint32_t bfm3:4;	// 12
    };
    uint32_t raw;
};
union bflnp_t {
    struct {
        uint32_t bfl0np:4;	// 0
        uint32_t bfl1np:4;	// 4
        uint32_t bfl2np:4;	// 8
        uint32_t bfl3np:4;	// 12
    };
    uint32_t raw;
};
union bfls_t {
    struct {
        uint32_t bfc0:1;	// 0
        uint32_t bfc1:1;	// 1
        uint32_t bfc2:1;	// 2
        uint32_t bfc3:1;	// 3
        uint32_t :12;
        uint32_t bfs0:1;	// 16
        uint32_t bfs1:1;	// 17
        uint32_t bfs2:1;	// 18
        uint32_t bfs3:1;	// 19
    };
    uint32_t raw;
};
union bound_t {
    struct {
        uint32_t boundary0:12;	// 0
        uint32_t :4;
        uint32_t boundary1:12;	// 16
    };
    uint32_t raw;
};
union cefclr_t {
    struct {
        uint32_t cev0:1;	// 0
        uint32_t cev1:1;	// 1
        uint32_t cev2:1;	// 2
        uint32_t cev3:1;	// 3
        uint32_t cev4:1;	// 4
        uint32_t cev5:1;	// 5
        uint32_t cev6:1;	// 6
        uint32_t cev7:1;	// 7
    };
    uint32_t raw;
};
union ceflag_t {
    struct {
        uint32_t cev0:1;	// 0
        uint32_t cev1:1;	// 1
        uint32_t cev2:1;	// 2
        uint32_t cev3:1;	// 3
        uint32_t cev4:1;	// 4
        uint32_t cev5:1;	// 5
        uint32_t cev6:1;	// 6
        uint32_t cev7:1;	// 7
    };
    uint32_t raw;
};
union cevnp0_t {
    struct {
        uint32_t cev0np:4;	// 0
        uint32_t cev1np:4;	// 4
        uint32_t cev2np:4;	// 8
        uint32_t cev3np:4;	// 12
        uint32_t cev4np:4;	// 16
        uint32_t cev5np:4;	// 20
        uint32_t cev6np:4;	// 24
        uint32_t cev7np:4;	// 28
    };
    uint32_t raw;
};
union chass_t {
    struct {
        uint32_t assch0:1;	// 0
        uint32_t assch1:1;	// 1
        uint32_t assch2:1;	// 2
        uint32_t assch3:1;	// 3
        uint32_t assch4:1;	// 4
        uint32_t assch5:1;	// 5
        uint32_t assch6:1;	// 6
        uint32_t assch7:1;	// 7
    };
    uint32_t raw;
};
union chctr_t {
    struct {
        uint32_t iclsel:2;	// 0
        uint32_t :2;
        uint32_t bndsell:2;	// 4
        uint32_t bndselu:2;	// 6
        uint32_t chevmode:2;	// 8
        uint32_t sync:1;	// 10
        uint32_t refsel:1;	// 11
        uint32_t :4;
        uint32_t resreg:4;	// 16
        uint32_t restbs:1;	// 20
        uint32_t respos:1;	// 21
        uint32_t :6;
        uint32_t bwdch:2;	// 28
        uint32_t bwden:1;	// 30
    };
    uint32_t raw;
};
union emuxctr_t {
    struct {
        uint32_t emuxset:3;	// 0
        uint32_t :5;
        uint32_t emuxact:3;	// 8
        uint32_t :5;
        uint32_t emuxch:10;	// 16
        uint32_t emuxmode:2;	// 26
        uint32_t emxcod:1;	// 28
        uint32_t emxst:1;	// 29
        uint32_t emxcss:1;	// 30
        uint32_t emxwc:1;	// 31
    };
    uint32_t raw;
};
union iclass_t {
    struct {
        uint32_t stcs:5;	// 0
        uint32_t :3;
        uint32_t cms:3;	// 8
        uint32_t :5;
        uint32_t stce:5;	// 16
        uint32_t :3;
        uint32_t cme:3;	// 24
    };
    uint32_t raw;
};
union q0r0_t {
    struct {
        uint32_t reqchnr:5;	// 0
        uint32_t rf:1;	// 5
        uint32_t ensi:1;	// 6
        uint32_t extr:1;	// 7
        uint32_t v:1;	// 8
    };
    uint32_t raw;
};
union qbur0_t {
    struct {
        uint32_t reqchnr:5;	// 0
        uint32_t rf:1;	// 5
        uint32_t ensi:1;	// 6
        uint32_t extr:1;	// 7
        uint32_t v:1;	// 8
    };
    uint32_t raw;
};
union qctrl0_t {
    struct {
        uint32_t srcresreg:4;	// 0
        uint32_t :4;
        uint32_t xtsel:4;	// 8
        uint32_t xtlvl:1;	// 12
        uint32_t xtmode:2;	// 13
        uint32_t xtwc:1;	// 15
        uint32_t gtsel:4;	// 16
        uint32_t gtlvl:1;	// 20
        uint32_t :2;
        uint32_t gtwc:1;	// 23
        uint32_t :4;
        uint32_t tmen:1;	// 28
        uint32_t :2;
        uint32_t tmwc:1;	// 31
    };
    uint32_t raw;
};
union qinr0_t {
    struct {
        uint32_t reqchnr:5;	// 0
        uint32_t rf:1;	// 5
        uint32_t ensi:1;	// 6
        uint32_t extr:1;	// 7
    };
    uint32_t raw;
};
union qmr0_t {
    struct {
        uint32_t engt:2;	// 0
        uint32_t entr:1;	// 2
        uint32_t :5;
        uint32_t clrv:1;	// 8
        uint32_t trev:1;	// 9
        uint32_t flush:1;	// 10
        uint32_t cev:1;	// 11
        uint32_t :4;
        uint32_t rptdis:1;	// 16
    };
    uint32_t raw;
};
union qsr0_t {
    struct {
        uint32_t fill:4;	// 0
        uint32_t :1;
        uint32_t empty:1;	// 5
        uint32_t :1;
        uint32_t reqgt:1;	// 7
        uint32_t ev:1;	// 8
    };
    uint32_t raw;
};
union rcr_t {
    struct {
        uint32_t :16;
        uint32_t drctr:4;	// 16
        uint32_t dmm:2;	// 20
        uint32_t :2;
        uint32_t wfr:1;	// 24
        uint32_t fen:2;	// 25
        uint32_t :4;
        uint32_t srgen:1;	// 31
    };
    uint32_t raw;
};
union refclr_t {
    struct {
        uint32_t rev0:1;	// 0
        uint32_t rev1:1;	// 1
        uint32_t rev2:1;	// 2
        uint32_t rev3:1;	// 3
        uint32_t rev4:1;	// 4
        uint32_t rev5:1;	// 5
        uint32_t rev6:1;	// 6
        uint32_t rev7:1;	// 7
        uint32_t rev8:1;	// 8
        uint32_t rev9:1;	// 9
        uint32_t rev10:1;	// 10
        uint32_t rev11:1;	// 11
        uint32_t rev12:1;	// 12
        uint32_t rev13:1;	// 13
        uint32_t rev14:1;	// 14
        uint32_t rev15:1;	// 15
    };
    uint32_t raw;
};
union reflag_t {
    struct {
        uint32_t rev0:1;	// 0
        uint32_t rev1:1;	// 1
        uint32_t rev2:1;	// 2
        uint32_t rev3:1;	// 3
        uint32_t rev4:1;	// 4
        uint32_t rev5:1;	// 5
        uint32_t rev6:1;	// 6
        uint32_t rev7:1;	// 7
        uint32_t rev8:1;	// 8
        uint32_t rev9:1;	// 9
        uint32_t rev10:1;	// 10
        uint32_t rev11:1;	// 11
        uint32_t rev12:1;	// 12
        uint32_t rev13:1;	// 13
        uint32_t rev14:1;	// 14
        uint32_t rev15:1;	// 15
    };
    uint32_t raw;
};
union res_t {
    struct {
        uint32_t result:16;	// 0
        uint32_t drc:4;	// 16
        uint32_t chnr:5;	// 20
        uint32_t emux:3;	// 25
        uint32_t crs:2;	// 28
        uint32_t fcr:1;	// 30
        uint32_t vf:1;	// 31
    };
    uint32_t raw;
};
union resd_t {
    struct {
        uint32_t result:16;	// 0
        uint32_t drc:4;	// 16
        uint32_t chnr:5;	// 20
        uint32_t emux:3;	// 25
        uint32_t crs:2;	// 28
        uint32_t fcr:1;	// 30
        uint32_t vf:1;	// 31
    };
    uint32_t raw;
};
union revnp0_t {
    struct {
        uint32_t rev0np:4;	// 0
        uint32_t rev1np:4;	// 4
        uint32_t rev2np:4;	// 8
        uint32_t rev3np:4;	// 12
        uint32_t rev4np:4;	// 16
        uint32_t rev5np:4;	// 20
        uint32_t rev6np:4;	// 24
        uint32_t rev7np:4;	// 28
    };
    uint32_t raw;
};
union revnp1_t {
    struct {
        uint32_t rev8np:4;	// 0
        uint32_t rev9np:4;	// 4
        uint32_t rev10np:4;	// 8
        uint32_t rev11np:4;	// 12
        uint32_t rev12np:4;	// 16
        uint32_t rev13np:4;	// 20
        uint32_t rev14np:4;	// 24
        uint32_t rev15np:4;	// 28
    };
    uint32_t raw;
};
union sefclr_t {
    struct {
        uint32_t sev0:1;	// 0
        uint32_t sev1:1;	// 1
    };
    uint32_t raw;
};
union seflag_t {
    struct {
        uint32_t sev0:1;	// 0
        uint32_t sev1:1;	// 1
    };
    uint32_t raw;
};
union sevnp_t {
    struct {
        uint32_t sev0np:4;	// 0
        uint32_t sev1np:4;	// 4
    };
    uint32_t raw;
};
union sract_t {
    struct {
        uint32_t agsr0:1;	// 0
        uint32_t agsr1:1;	// 1
        uint32_t agsr2:1;	// 2
        uint32_t agsr3:1;	// 3
        uint32_t :4;
        uint32_t assr0:1;	// 8
        uint32_t assr1:1;	// 9
        uint32_t assr2:1;	// 10
        uint32_t assr3:1;	// 11
    };
    uint32_t raw;
};
union synctr_t {
    struct {
        uint32_t stsel:2;	// 0
        uint32_t :2;
        uint32_t evalr1:1;	// 4
        uint32_t evalr2:1;	// 5
        uint32_t evalr3:1;	// 6
    };
    uint32_t raw;
};
union vfr_t {
    struct {
        uint32_t vf0:1;	// 0
        uint32_t vf1:1;	// 1
        uint32_t vf2:1;	// 2
        uint32_t vf3:1;	// 3
        uint32_t vf4:1;	// 4
        uint32_t vf5:1;	// 5
        uint32_t vf6:1;	// 6
        uint32_t vf7:1;	// 7
        uint32_t vf8:1;	// 8
        uint32_t vf9:1;	// 9
        uint32_t vf10:1;	// 10
        uint32_t vf11:1;	// 11
        uint32_t vf12:1;	// 12
        uint32_t vf13:1;	// 13
        uint32_t vf14:1;	// 14
        uint32_t vf15:1;	// 15
    };
    uint32_t raw;
};
}
namespace wdt_ns {
union id_t {
    struct {
        uint32_t mod_rev:8;	// 0
        uint32_t mod_type:8;	// 8
        uint32_t mod_number:16;	// 16
    };
    uint32_t raw;
};
}
