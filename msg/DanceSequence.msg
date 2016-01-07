float32 rotAccel      # rotational acceleration in degrees / sec
float32 rotDecel      # rotational decceleration in degrees / sec

float32 transAccel    # translational acceleration in mm / sec
float32 transDecel    # translational decceleration in mm / sec

float32 DEFAULT = 0.0     # use default value for Accel, Deceel

uint8 ROTATION = 0      # to define each mode in danceStep[]
uint8 TRANSLATION = 1   # to define each mode in danceStep[]

sf_dance/DanceStep[] danceStep # the dance step
