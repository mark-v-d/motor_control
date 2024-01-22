[FILTER]
	PROGRAM_EXTENSION = .png,.gif,.jpg Grayscale Depth Image
	PROGRAM_EXTENSION = .py Python Script
	png = image-to-gcode
	gif = image-to-gcode
	jpg = image-to-gcode
	py = python

[XHC_HB04_CONFIG]
	# specify layout = n for LIB:xhc-hb04-layout{n}.cfg files
	layout = 2
	# coords: specify upto 4 letters from set {x y z a b c u v w}
	# coords: switch labels are xyza but any unique 4 letters
	#         from the set {xyxabcuvw) can be used (if the coord exists)
	coords = x z
	# lowpass settings:
	# coef: slows rate of change of output,  range: 0 < coef < 1
	coefs = 1 1 1 1
	# scale plus or minus, rotaries may require larger scale factor:
	scales = -1 1 1 1
	# jogmode normal(default) or vnormal
	jogmode = normal
	# sequence 1: 0.001,0.010,0.100,1.000 (mm-based machine)
	# sequence 2: 0.001,0.005,0.010,0.020 (inch-based machine)
	# sequence 3: 0.001,0.010,0.100
	# sequence 4: 0.001,0.005,0.010,0.020,0.050,0.100
	# sequence 5: 0.001,0.010,0.050,0.100,1.000
	sequence = 1
	# require_pendant = yes|no: create xhc-hb04 hal pins when pendant not
	#                           connected at startup
	require_pendant = no
	# inch_or_mm = in | mm
	inch_or_mm = mm
	# mpg_accels: provision for reduced acceleration for manual mode jogging
	mpg_accels = 300 300 300 300

[APPLICATIONS]
	APP = monitor-xhc-hb04

[RS274NGC]
	PARAMETER_FILE = lathe.var
	RS274NGC_STARTUP_CODE = G18 G21 G40 G49 G54 G80 G90 G94 G7 M9 M5 G64 P0.001


[EMCMOT]
	EMCMOT = motmod
	COMM_TIMEOUT = 1.0
	SERVO_PERIOD = 222222

[TASK]
	TASK = milltask
	CYCLE_TIME = 0.001

[TRAJ]
	COORDINATES = X Z
	LINEAR_UNITS = mm
	ANGULAR_UNITS = degree
	DEFAULT_LINEAR_VELOCITY = 5.0
	MAX_LINEAR_VELOCITY = 50
	DEFAULT_LINEAR_ACCELERATION = 1800.0
	MAX_LINEAR_ACCELERATION = 2000.0

[EMCIO]
	EMCIO = io
	CYCLE_TIME = 0.100
	TOOL_TABLE = lathe.tbl
	#TOOL_CHANGE_POSITION = 2 0 1
	TOOL_CHANGE_WITH_SPINDLE_ON = 1

[KINS]
	KINEMATICS = trivkins coordinates=xz
	JOINTS = 2

[AXIS_X]
	MIN_LIMIT = -1.0
	MAX_LIMIT = 130.0
	MAX_VELOCITY = 50
	MAX_ACCELERATION = 2000.0

[JOINT_0]
	TYPE = LINEAR
	MAX_VELOCITY = 50
	MAX_ACCELERATION = 2000
	MIN_LIMIT = -1.0
	MAX_LIMIT = 140.0
	HOME_SEQUENCE = 0
	HOME = 129.0
	HOME_OFFSET = 130.0
	HOME_SEARCH_VEL = 5
	HOME_LATCH_VEL =  -20
	HOME_USE_INDEX =  YES
	HOME_IGNORE_LIMITS = NO

[AXIS_Z]
	MIN_LIMIT = -5.0
	MAX_LIMIT = 520.0
	MAX_VELOCITY = 50
	MAX_ACCELERATION = 2000.0

[JOINT_1]
	TYPE = LINEAR
	MAX_VELOCITY = 50
	MAX_ACCELERATION = 2000
	MIN_LIMIT = -5.0
	MAX_LIMIT = 520.0
	HOME_SEQUENCE = 1
	HOME=0
	HOME_OFFSET =    0.0
	HOME_SEARCH_VEL = -25
	HOME_LATCH_VEL =   20
	HOME_USE_INDEX =   YES
	HOME_IGNORE_LIMITS = NO

