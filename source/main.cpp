#include <stdio.h>
#include "pico/stdlib.h"
#include "ei_run_classifier.h"

const uint LED_PIN = 25;

static const float samples[][EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = {
{1.47186603008024, 1.43644299030407, 1.54408538122891, 1.44192106155546, 1.47627659016337, 1.30951931854065, 1.39000590932632, 1.45580905111283,1.42194186801269, 1.45352246443474, 1.41034655730267, 1.37991042868484, 1.3815324010495, 1.40956071210045, 1.31954830283628, 2.5154237191066, 2.46781487875025, 2.50158194074487, 2.42511954340302, 2.37789196584191, 2.27230435217329, 2.28894831413948, 2.36371920550661, 2.38182467313411,2.39170643864444, 2.4126661841667, 2.36413272038861, 2.33731437306998, 2.26405302480667, 2.33757963762861}, // Sample 1
{0.321950761470946, 0.473604977482588, 0.240904356674499, 0.566512898053711, 1.38073300042768, 1.88712509687805,2.1518325779338, 2.32861738962414, 2.7989148217466, 3.20151873391347, 3.67186932328583, 3.86252878587486, 3.71472398073108,3.21161768122316, 2.67105909484101, 0.0, 0.656535124978769, 0.397277383034222, 0.969015837038405, 0.0, 0.0, 3.60089356695792,0.0, 0.0, 0.0, 5.99114557462732, 0.0, 6.25632772731152, 0.0, 4.54489401234073}, // Sample 2
{5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 3.64265086496608, 2.80689043112803, 3.24428530667554, 2.67665590560404,1.38981048061405, 0.466797891127744, 0.549902869308496, 0.491512665026944,0.509606601333618, 0.691210347870976, 0.600504053583338, 0.803277502269022, 0.809173625565665, 0.908792078967269, 0.935443356368602},
{2.08525232893749, 2.15269936122036, 2.11555407706624, 1.94082490834079, 2.03273058885906, 2.01961137649556, 2.02333567043655, 1.88174297672526, 1.90337001966038, 1.86553414111357, 1.82554355916133, 1.73726189165098, 1.7094885887612, 1.79289899672443, 1.70231459770277, 0.418024285312847, 0.435012824200057, 0.403191891350102, 0.398028768495541, 0.399986491149712, 0.407486112762545, 0.396574472597599, 0.40110982748815, 0.385024364656552, 0.387996280699429, 0.366591906939125, 0.360706155516247, 0.345983601048758, 0.358299006512687, 0.350999313998177},
{0.827444527953008, 0.866585469325516, 0.804181221370171, 0.855626054447124, 0.0, 0.0, 0.761298456837887, 0.0, 0.0, 0.0, 0.0, 0.822335235915186, 0.0, 0.769839754769332, 0.834882777835495, 1.64269522491549, 1.73998514295185, 1.57653118565829, 1.50793274265388, 1.58982124018233, 1.73174224185149, 1.70333627257479, 1.64208467059966, 1.50290092168777, 1.62869703987184, 1.62522933015449, 1.53561080812312, 1.68216114567812, 1.65088901480408, 1.63719673306087},
{2.96438358708858, 2.91181787544983, 2.98396512476132, 2.95927770215567, 2.99241832158188, 3.10779074149708, 2.99591393522444, 2.95707971020605, 2.87915299891641, 2.91754673937341, 2.94400844145976, 3.05627696714815, 2.94830556633008, 2.85885283893389, 2.85858982837497, 0.744962027342025, 0.750203051135203, 0.721549130891922, 0.741233176235833, 0.755753561438878, 0.750565864530504, 0.748236375252663, 0.762184155006724, 0.751653323100861, 0.736516457222934, 0.735752378989934, 0.734334671386033, 0.753467278858169, 0.744230183348482, 0.740773962874125},
{1.09568882219053, 1.04936675434888, 1.11199048140446, 1.03757430543888, 1.06288751860148, 0.996606285260879, 1.09707660787174, 1.05269767379862, 1.04597484710734, 1.00734993783927, 1.04972956611254, 1.02537228417503, 0.996153640208626, 1.01562492349961, 1.03367161464096, 3.02069843594823, 3.35648595474787, 2.6647264928721, 2.35286206313676, 2.60038846010221, 2.14297630994404, 2.71320544226649, 2.11570484880103, 2.95669386114948, 2.46879114888203, 2.39062223297949, 1.59489378349729, 2.95572321074907, 3.11580358994124, 2.52656244581551},
{1.55958351574619, 1.44734429767433, 1.58686454410011, 1.51975293156358, 1.49355345780863, 1.57161971668976, 1.55452219213212, 1.60044591729845, 1.80667932216811, 2.01868026464394, 2.03164446708635, 2.03869922697829, 1.98302436993967, 1.77330841960372, 1.75951024860501, 3.04075598344453, 3.03598732197386, 3.10987803271844, 3.09641959732133, 3.07355562836474, 3.05223739228895, 3.19228528365716, 3.23059632925404, 3.61717889248487, 3.93716455246838, 3.90414908128261, 4.01565164860739, 3.91754237483615, 3.5373529737009, 3.54649715087138},
{0.0, 0.954830786828162, 0.929732151556828, 1.03022710286226, 0.0, 0.898580435195953, 0.885059516545838, 0.965834201374534, 0.0, 0.939284971200714, 0.885677812483839, 0.0, 1.02218592891669, 0.971456776229927, 0.959492208285606, 1.98602089622454, 1.93627147757705, 1.89821180480606, 1.9540014081585, 1.93978411345035, 1.92954867963858, 1.8774699989876, 1.96065668879592, 1.9475708916841, 1.95531405474495, 1.85885336372211, 1.90650806070471, 1.89120498236335, 1.96046803139742, 1.95697931183289},
{5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 0.0895358875731034, 0.0513277727618333, -0.0312645401102745, -0.023625551186621, -0.0179171261283567, 0.0105889106562686, 0.0286120162408348, -0.0951677786531967, -0.0674668591151046, -0.00746606801597692, -0.0396601035476502, -0.0119632005530282, 0.0161424460192867, -0.0376316711797285, -0.0201912864799738},
};

static const float true_results[] = {0, 0, 0, 0, 0, 0 ,0 ,1 ,0 ,0};
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
    int correct_predictions = 0; 
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
        features_signal.get_data = [sampleIdx](size_t offset, size_t length, float *out_ptr) -> int {
            return raw_feature_get_data(sampleIdx, offset, length, out_ptr);
        };

        
        ei_printf("Edge Impulse standalone inferencing (Raspberry Pico 2040)\n");
        ei_printf("Processing sample index: %zu\n", sampleIdx);
        gpio_put(LED_PIN, 0);
        EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
        ei_printf("run_classifier returned: %d\n", res);

        if (res != 0) return res;

        ei_printf("Predictions ");
        ei_printf("(DSP: %d ms., Classification latency: %d ms., Anomaly: %d ms.)",
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
            if (result.classification[ix].value > 0.5 && true_results[sampleIdx] == ix) { 
                correct_predictions++; 
            }
        }
        
        ei_printf(" (True result: %.1f)", true_results[sampleIdx]);
        float accuracy = (float)correct_predictions / (sizeof(true_results) / sizeof(true_results[0])) * 100.0f;
        ei_printf("Accuracy: %.2f%%\n", accuracy);
#if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif
        gpio_put(LED_PIN, 1);
        sleep_ms(1000);
    }
    float accuracy = (float)correct_predictions / (sizeof(true_results) / sizeof(true_results[0])) * 100.0f;
    ei_printf("Accuracy: %.2f%%\n", accuracy);
}

    return 0;
}
