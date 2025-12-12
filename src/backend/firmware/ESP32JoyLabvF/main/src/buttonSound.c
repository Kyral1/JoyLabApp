#include "buttonSound.h"
#include "Speaker_Control.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <string.h>
#include "esp_log.h"

static const char *TAG = "BUTTON_SOUND";

static char button_sounds[NUM_BUTTONS][MAX_SOUND_NAME_LEN] = {
    "HappyNoise.wav", "HappyNoise.wav", "HappyNoise.wav", "HappyNoise.wav"
};

void save_button_sound(int button, const char *filename){
    if (button < 0 || button >= NUM_BUTTONS) return;
    strncpy(button_sounds[button], filename, MAX_SOUND_NAME_LEN);
}

const char* get_button_sound(int button)
{
    if (button < 0 || button >= NUM_BUTTONS) return "";
    return button_sounds[button];
}

//save
void save_button_sound_persistent(int button, const char *filename)
{
    if (button < 0 || button >= NUM_BUTTONS) return;

    nvs_handle_t nvs;
    if (nvs_open("sound_states", NVS_READWRITE, &nvs) == ESP_OK)
    {
        char key[8];
        sprintf(key, "snd%d", button);

        nvs_set_str(nvs, key, filename);
        nvs_commit(nvs);
        nvs_close(nvs);
    }

    save_button_sound(button, filename);
}

//load from nvs
void load_button_sounds_from_nvs(void)
{
    nvs_handle_t nvs;
    if (nvs_open("sound_states", NVS_READONLY, &nvs) != ESP_OK)
        return;

    for (int i = 0; i < NUM_BUTTONS; i++)
    {
        char key[8];
        sprintf(key, "snd%d", i);

        size_t len = MAX_SOUND_NAME_LEN;
        if (nvs_get_str(nvs, key, button_sounds[i], &len) == ESP_OK)
        {
            ESP_LOGI(TAG, "Button %d sound loaded: %s", i, button_sounds[i]);
        }
    }

    nvs_close(nvs);
}

//play sound
void play_button_sound(int button)
{
    if (button < 0 || button >= NUM_BUTTONS) return;

    char path[64];
    snprintf(path, sizeof(path), "/spiffs/%s", button_sounds[button]);

    ESP_LOGI(TAG, "Playing: %s", path);
    speaker_play_wav(path);
}