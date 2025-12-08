file(GLOB srcs
    "main.c"
    "src/*.c"
    "BMI323/*.c"
)

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "." "include" "BMI323"
)
