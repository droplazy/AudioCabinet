
/*
* Copyright (c) 2018-2022 Allwinner Technology Co. Ltd. All rights reserved.
* Author: BT Team
* Date: 2022.03.12
*/

#ifndef __BT_ALSA_H
#define __BT_ALSA_H
#if __cplusplus
extern "C" {
#endif
#include <alsa/asoundlib.h>
#include <alsa/pcm_external.h>
#include <stdbool.h>
#define PCM_DEVICE_LEN 48

extern int  occurredFlag;

struct pcm_config {
    char device[PCM_DEVICE_LEN];
    bool drop;
    snd_pcm_t *pcm;
    snd_pcm_stream_t stream;
    uint8_t channels; // channels num : 1
    uint32_t sampling; // Sampling Rate : 16000 | 44100 | 48000
    uint16_t bit_p_spl;
    unsigned int buffer_time;
    unsigned int period_time;
    snd_pcm_format_t format;
    ssize_t format_size;
};

int aw_pcm_open(struct pcm_config *pf);
int aw_pcm_write(snd_pcm_t *pcm, char *buffer, int frames);
int aw_pcm_read(snd_pcm_t *pcm, char *buffer, int frames);
int aw_pcm_get_params(struct pcm_config *pf, snd_pcm_uframes_t *buff_size,
                      snd_pcm_uframes_t *period_size);
void aw_pcm_free(struct pcm_config *pf);
int bt_alsa_dump_pcm(int *fd, const char *path, char *buff, uint32_t len, uint32_t max_len);
#if __cplusplus
}; // extern "C"
#endif

#endif
