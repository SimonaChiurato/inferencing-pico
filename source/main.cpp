#include <stdio.h>
#include "pico/stdlib.h"
#include "ei_run_classifier.h"

const uint LED_PIN = 25;

static const float samples[][EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = {
    {1.47186603008024, 1.43644299030407, 1.54408538122891, 1.44192106155546, 1.47627659016337, 1.30951931854065, 1.39000590932632, 1.45580905111283,
1.42194186801269, 1.45352246443474, 1.41034655730267, 1.37991042868484, 1.3815324010495, 1.40956071210045, 1.31954830283628, 2.5154237191066, 2.46781487875025, 
2.50158194074487, 2.42511954340302, 2.37789196584191, 2.27230435217329, 2.28894831413948, 2.36371920550661, 2.38182467313411,
2.39170643864444, 2.4126661841667, 2.36413272038861, 2.33731437306998, 2.26405302480667, 2.33757963762861}, // Sample 1
    {0.321950761470946, 0.473604977482588, 0.240904356674499, 0.566512898053711, 1.38073300042768, 1.88712509687805,
2.1518325779338, 2.32861738962414, 2.7989148217466, 3.20151873391347, 3.67186932328583, 3.86252878587486, 3.71472398073108,
3.21161768122316, 2.67105909484101, 0.0, 0.656535124978769, 0.397277383034222, 0.969015837038405, 0.0, 0.0, 3.60089356695792,
0.0, 0.0, 0.0, 5.99114557462732, 0.0, 6.25632772731152, 0.0, 4.54489401234073}, // Sample 2
{5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 3.64265086496608, 2.80689043112803, 3.24428530667554, 2.67665590560404,
1.38981048061405, 0.466797891127744, 0.549902869308496, 0.491512665026944,
0.509606601333618, 0.691210347870976, 0.600504053583338, 0.803277502269022, 0.809173625565665, 0.908792078967269, 0.935443356368602},
};
/*
static const float true_results[][2] = {
    {1.0, 0.0},
    {1.0, 0.0},
    {1.0, 0.0},
};*/
int raw_feature_get_data(size_t sampleIndex, size_t offset, size_t length, float *out_ptr) {
    if (sampleIndex >= sizeof(samples) / sizeof(samples[0])) {
        return -1; // Error: Invalid sample index
    }
    memcpy(out_ptr, samples[sampleIndex] + offset, length * sizeof(float));
    return 0;
}

int main() {
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    gpio_put(LED_PIN, 1);
    sleep_ms(250);
    gpio_put(LED_PIN, 0);
    sleep_ms(250);
while(true) {
    for (size_t sampleIdx = 0; sampleIdx < sizeof(samples) / sizeof(samples[0]); sampleIdx++) {
        
        if (sizeof(samples[sampleIdx]) / sizeof(float) != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
            ei_printf("The size of your 'features' array is not correct. Expected %lu items, but had %lu\n",
                EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, sizeof(samples[sampleIdx]) / sizeof(float));
            sleep_ms(1000);
            return -1;
        }

        ei_impulse_result_t result = {0};
        
        signal_t features_signal;
        features_signal.total_length = sizeof(samples[sampleIdx]) / sizeof(samples[0][0]);
        features_signal.get_data = [sampleIdx](size_t offset, size_t length, float *out_ptr) {
            return raw_feature_get_data(sampleIdx, offset, length, out_ptr);
        };
        
        ei_printf("Edge Impulse standalone inferencing (Raspberry Pico 2040)\n");
        gpio_put(LED_PIN, 0);
        EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
        ei_printf("run_classifier returned: %d\n", res);

        if (res != 0) return res;

        ei_printf("Predictions ");
        ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
            result.timing.dsp, result.timing.classification, result.timing.anomaly);
        ei_printf(": \n");
        ei_printf("[");
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            ei_printf("%.5f", result.classification[ix].value);
            if (ix != EI_CLASSIFIER_LABEL_COUNT - 1) {
                ei_printf(", ");
            }
        }
        ei_printf("]\n");

        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            ei_printf("    %s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
            ei_printf(" (true result: %.1f, %.1f)", true_results[ix][0], true_results[ix][1]);
        }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif
        gpio_put(LED_PIN, 1);
        sleep_ms(1000);
    }
}

    return 0;
}
