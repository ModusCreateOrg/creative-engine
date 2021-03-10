#ifndef NETWORK_DISPLAY_CLIENT_NETWORKDISPLAYCONFIG_H
#define NETWORK_DISPLAY_CLIENT_NETWORKDISPLAYCONFIG_H


#include "ini/ini.h"


#define MATCH_GROUP(s) strcmp(aSection, s) == 0
#define MATCH_CONFIG(s) strcmp(aName, s) == 0

static int ini_file_handler(void* aConfig, const char* aSection, const char* aName, const char* aValue);


typedef struct {
  char *destinationPort;
  char *destinationIp;
} NetworkDisplaySegment;

typedef struct NetworkDisplayConfig {
  uint16_t inputScreenWidth;
  uint16_t inputScreenHeight;

  uint16_t outputScreenWidth;
  uint16_t outputScreenHeight;

  uint16_t singlePanelWidth;
  uint16_t singlePanelHeight;


  uint16_t numberSegments;
  uint16_t segmentWidth;
  uint16_t segmentHeight;
  uint16_t frameRate;

  uint16_t destinationIpStartDigit;

  NetworkDisplaySegment *segments;

  void InitSegments(uint16_t aNumSegments) {
    numberSegments = aNumSegments;
    segments = (NetworkDisplaySegment *)malloc(numberSegments * sizeof(NetworkDisplaySegment));
  }


  void Describe() {
    printf("NetworkDisplayConfig: \n");

    printf("\tframeRate: %i\n", frameRate);
    printf("\tinputStreamWidth: %i\n", inputScreenWidth);
    printf("\tinputStreamHeight: %i\n", inputScreenHeight);

    printf("\toutputScreenWidth: %i\n", outputScreenWidth);
    printf("\toutputScreenHeight: %i\n", outputScreenHeight);
    printf("\tsinglePanelWidth: %i\n", singlePanelWidth);
    printf("\tsinglePanelHeight: %i\n", singlePanelHeight);
    printf("\tnumberSegments: %i\n", numberSegments);
    printf("\tsegmentWidth: %i\n", segmentWidth);
    printf("\tsegmentHeight: %i\n", segmentHeight);
    printf("\tSegments (%i):\n", numberSegments);

    for (int i = 0; i < numberSegments; i++) {
      printf("\t\tSegment[%i] IP:%s, Port: %s\n", i, segments[i].destinationIp, segments[i].destinationPort);
    }
    fflush(stdout);
  }
} NetworkDisplayConfig;


// Parses a config from the .ini file.
static int ini_file_handler(void* aConfig, const char* aSection, const char* aName, const char* aValue) {

  auto *displayConfig = (NetworkDisplayConfig*)aConfig;
  printf("displayConfig %p\n", displayConfig);

  if (MATCH_GROUP("input_screen")) {
    if (MATCH_CONFIG("input_screen_width")) {
      displayConfig->inputScreenWidth = atoi(aValue);
      return 1;
    }
    if (MATCH_CONFIG("input_screen_height")) {
      displayConfig->inputScreenHeight = atoi(aValue);
      return 1;
    }
    if (MATCH_CONFIG("frame_rate")) {
      displayConfig->frameRate = atoi(aValue);
      return 1;
    }
    return 0;
  }

  // This is mostly for debugging purposes and doesn't get used in any calculations
  if (MATCH_GROUP("matrix_dimensions")) {
    if (MATCH_CONFIG("width")) {
      displayConfig->singlePanelWidth = atoi(aValue);
      return 1;
    }
    if (MATCH_CONFIG("height")) {
      displayConfig->singlePanelHeight = atoi(aValue);
      return 1;
    }
    return 0;
  }

  if (MATCH_GROUP("segment_info")) {
    if (MATCH_CONFIG("number_segments")) {
      displayConfig->InitSegments(atoi(aValue));
      return 1;
    }
    if (MATCH_CONFIG("segment_width")) {
      displayConfig->segmentWidth = atoi(aValue);
      return 1;
    }
    if (MATCH_CONFIG("segment_height")) {
      displayConfig->segmentHeight = atoi(aValue);
      return 1;
    }

    return 0;
  }

  if (MATCH_GROUP("output_screen")) {
    if (MATCH_CONFIG("output_screen_width")) {
      displayConfig->outputScreenWidth = atoi(aValue);
      return 1;
    }
    if (MATCH_CONFIG("output_screen_height")) {
      displayConfig->outputScreenHeight = atoi(aValue);
      return 1;
    }

    return 0;
  }


//   Found a segment
  if (strstr(aSection, "segment_num_") != NULL) {
    int segmentNum = -1;

    char *buff = strdup(aSection);
    char *token;

    int i = 0;
    while ((token = strsep(&buff, "_")) != NULL) {
      if (i == 2) {
        segmentNum = atoi(token);
        break;
      }

      i++;
    }

    if (segmentNum != -1) {
      // Too many segments!
      if (segmentNum > displayConfig->numberSegments) {
        fprintf(
          stderr,
          "Fatal error! Segment %i found in ini file, but greater than total segments of %i.\n",
          segmentNum,
          displayConfig->numberSegments
        );

        return 1;
      }

      if (MATCH_CONFIG("ip")) {
        displayConfig->segments[segmentNum - 1].destinationIp = strdup(aValue);
        return 1;
      }
      if (MATCH_CONFIG("port")) {
        displayConfig->segments[segmentNum - 1].destinationPort = strdup(aValue);
        return 1;
      }
    }
    else {
      fprintf(stderr, "Fatal Error! Could not find segment number in group [%s]\n", aSection);
      return 0;
    }

  }
  return 0;
}




#endif //NETWORK_DISPLAY_CLIENT_NETWORKDISPLAYCONFIG_H
