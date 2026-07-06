#ifndef FRONT_END_CEL_RUNTIME_H
#define FRONT_END_CEL_RUNTIME_H

#include "graphics.h"

#define CANS_CHUNK_TAG_ANIM 0x414E494DU
#define CANS_CHUNK_TAG_AACL 0x4141434CU
#define CANS_CHUNK_TAG_CCB  0x43434220U
#define CANS_CHUNK_TAG_CTPT 0x43545054U
#define CANS_CHUNK_TAG_HSPT 0x48535054U
#define CANS_CHUNK_TAG_OFST 0x4F465354U
#define CANS_CHUNK_TAG_OFSS 0x4F465353U
#define CANS_CHUNK_TAG_PDAT 0x50444154U
#define CANS_CHUNK_TAG_PLUT 0x504C5554U
#define CANS_CHUNK_TAG_RPDT 0x52504454U
#define CANS_RESOURCE_TYPE_CEL 0x43454C20U
#define CANS_RESOURCE_TYPE_PALETTE 0x50414C20U
#define CANS_CHUNK_TAG_PREFIX_MASK 0xFF000000U

#define CANS_METADATA_FRAME (-1)
#define CANS_CENTER_ENCODING_WIDE 0
#define CANS_HOTSPOT_RECORD_COUNT_SHIFT 24
#define CANS_HOTSPOT_CENTER_PRESENT 0x00010000U
#define CANS_HOTSPOT_ENDPOINT_STRIDE 2
#define CANS_MAX_HOTSPOT_VECTORS 2
#define CANS_HOTSPOT_COORDINATE_SCALE 0x200
#define CANS_CCB_DIMENSION_EXPONENT_MASK 0x1FU
#define CANS_CCB_DIMENSION_LITERAL_MASK 0x1FFU
#define CANS_WORD_SIGN_BIT_SHIFT 31

typedef struct FrontEndCelOffset
{
  int x;
  int y;
} FrontEndCelOffset;

typedef struct CansHotspotVector
{
  int start_x;
  int start_y;
  int end_x;
  int end_y;
} CansHotspotVector;

typedef union CansAnimationAuxiliaryOutput
{
  FrontEndCelOffset *hotspots;
  CansHotspotVector hotspot_vectors[CANS_MAX_HOTSPOT_VECTORS];
} CansAnimationAuxiliaryOutput;

typedef struct CansAnimationFrameOutput
{
  CCB *ccb;
  void *plut;
  CelData *pixel_data;
  int center_x;
  int center_y;
  int hotspot_count;
  CansAnimationAuxiliaryOutput auxiliary;
  int animation_flags;
} CansAnimationFrameOutput;

typedef void (*CansAnimationFrameVisitor)(
    const CansAnimationFrameOutput *output,
    void                           *context);

typedef struct CansAnimationData
{
  uint32 tag;
} CansAnimationData;

typedef struct CansChunkHeader
{
  uint32 tag;
  uint32 byte_count;
} CansChunkHeader;

typedef struct CansFileOffsetChunk
{
  uint32 tag;
  uint32 animation_offset;
} CansFileOffsetChunk;

typedef struct CansPixelDataChunk
{
  uint32 tag;
  uint32 byte_count;
  uint32 pixels[1];
} CansPixelDataChunk;

typedef struct CansPackedPosition
{
  uint16 y;
  uint16 x;
} CansPackedPosition;

typedef struct CansCompactCenterPair
{
  uint8 even_x;
  uint8 even_y;
  uint8 odd_x;
  uint8 odd_y;
} CansCompactCenterPair;

typedef union CansCenterPointData
{
  CansPackedPosition wide[1];
  CansCompactCenterPair compact[1];
} CansCenterPointData;

typedef struct CansCenterPointChunk
{
  uint32 tag;
  uint32 byte_count;
  int32 frame_count;
  int32 encoding;
  CansCenterPointData centers;
} CansCenterPointChunk;

typedef struct CansHotspotRecord
{
  uint32 control;
  CansPackedPosition position;
} CansHotspotRecord;

typedef union CansHotspotCount
{
  int32 encoded_frame_count;
  int32 decoded_hotspot_count;
} CansHotspotCount;

typedef struct CansHotspotChunk
{
  uint32 tag;
  uint32 byte_count;
  CansHotspotCount count;
  int32 relocation_state;
  CansHotspotRecord records[1];
} CansHotspotChunk;

typedef struct CansReducedPixelDataChunk
{
  uint32 tag;
  uint32 byte_count;
  int width_preamble_index;
  int height_preamble_index;
  uint32 pixels[1];
} CansReducedPixelDataChunk;

typedef struct CansLegacyReducedPixelDataChunk
{
  uint32 tag;
  uint32 byte_count;
  int16 top;
  int16 left;
  int16 bottom;
  int16 right;
  uint32 pixels[1];
} CansLegacyReducedPixelDataChunk;

typedef struct CansCcbChunk
{
  uint32 tag;
  uint32 byte_count;
  uint32 version;
  CCB ccb;
} CansCcbChunk;

typedef struct CansPlutChunk
{
  uint32 tag;
  uint32 byte_count;
  int entry_count;
  uint16 entries[1];
} CansPlutChunk;

typedef struct CansAaclChunk
{
  uint32 tag;
  uint32 byte_count;
  int32 ccb_offset;
  int32 plut_offset;
  uint8 payload[1];
} CansAaclChunk;

typedef struct CansOffsetAnimation
{
  uint32 tag;
  CansReducedPixelDataChunk **frame_chunks;
  FrontEndCelOffset **frame_centers;
  FrontEndCelOffset **frame_hotspots;
  CansCcbChunk *ccb_chunk;
  CansPlutChunk *plut_chunk;
} CansOffsetAnimation;

typedef struct CansAnimationMetadata
{
  CCB *ccb;
  void *plut;
  CelData *pixel_data;
  int center_x;
  int center_y;
  int hotspot_count;
  FrontEndCelOffset *hotspots;
} CansAnimationMetadata;

typedef struct FrontEndCelResourceInfo
{
  int x;
  int y;
  CansAnimationMetadata animation;
} FrontEndCelResourceInfo;

typedef char FrontEndCelOffsetSizeCheck[
    (sizeof(FrontEndCelOffset) == 8) ? 1 : -1];
typedef char CansHotspotVectorSizeCheck[
    (sizeof(CansHotspotVector) == 0x10) ? 1 : -1];
typedef char CansAnimationAuxiliaryOutputSizeCheck[
    (sizeof(CansAnimationAuxiliaryOutput) == 0x20) ? 1 : -1];
typedef char CansAnimationFrameOutputSizeCheck[
    (sizeof(CansAnimationFrameOutput) == 0x3C) ? 1 : -1];
typedef char CansAnimationFrameOutputCcbOffsetCheck[
    (offsetof(CansAnimationFrameOutput, ccb) == 0x00) ? 1 : -1];
typedef char CansAnimationFrameOutputPlutOffsetCheck[
    (offsetof(CansAnimationFrameOutput, plut) == 0x04) ? 1 : -1];
typedef char CansAnimationFrameOutputPixelDataOffsetCheck[
    (offsetof(CansAnimationFrameOutput, pixel_data) == 0x08) ? 1 : -1];
typedef char CansAnimationFrameOutputCenterXOffsetCheck[
    (offsetof(CansAnimationFrameOutput, center_x) == 0x0C) ? 1 : -1];
typedef char CansAnimationFrameOutputCenterYOffsetCheck[
    (offsetof(CansAnimationFrameOutput, center_y) == 0x10) ? 1 : -1];
typedef char CansAnimationFrameOutputHotspotCountOffsetCheck[
    (offsetof(CansAnimationFrameOutput, hotspot_count) == 0x14) ? 1 : -1];
typedef char CansAnimationFrameOutputAuxiliaryOffsetCheck[
    (offsetof(CansAnimationFrameOutput, auxiliary) == 0x18) ? 1 : -1];
typedef char CansAnimationFrameOutputFlagsOffsetCheck[
    (offsetof(CansAnimationFrameOutput, animation_flags) == 0x38) ? 1 : -1];
typedef char CansAnimationDataSizeCheck[
    (sizeof(CansAnimationData) == 0x04) ? 1 : -1];
typedef char CansChunkHeaderSizeCheck[
    (sizeof(CansChunkHeader) == 0x08) ? 1 : -1];
typedef char CansFileOffsetChunkSizeCheck[
    (sizeof(CansFileOffsetChunk) == 0x08) ? 1 : -1];
typedef char CansPixelDataOffsetCheck[
    (offsetof(CansPixelDataChunk, pixels) == 0x08) ? 1 : -1];
typedef char CansPackedPositionSizeCheck[
    (sizeof(CansPackedPosition) == 0x04) ? 1 : -1];
typedef char CansCompactCenterPairSizeCheck[
    (sizeof(CansCompactCenterPair) == 0x04) ? 1 : -1];
typedef char CansCenterPointDataOffsetCheck[
    (offsetof(CansCenterPointChunk, centers) == 0x10) ? 1 : -1];
typedef char CansHotspotRecordSizeCheck[
    (sizeof(CansHotspotRecord) == 0x08) ? 1 : -1];
typedef char CansHotspotRecordDataOffsetCheck[
    (offsetof(CansHotspotChunk, records) == 0x10) ? 1 : -1];
typedef char CansLegacyReducedTopOffsetCheck[
    (offsetof(CansLegacyReducedPixelDataChunk, top) == 0x08) ? 1 : -1];
typedef char CansLegacyReducedLeftOffsetCheck[
    (offsetof(CansLegacyReducedPixelDataChunk, left) == 0x0A) ? 1 : -1];
typedef char CansLegacyReducedBottomOffsetCheck[
    (offsetof(CansLegacyReducedPixelDataChunk, bottom) == 0x0C) ? 1 : -1];
typedef char CansLegacyReducedRightOffsetCheck[
    (offsetof(CansLegacyReducedPixelDataChunk, right) == 0x0E) ? 1 : -1];
typedef char CansLegacyReducedPixelsOffsetCheck[
    (offsetof(CansLegacyReducedPixelDataChunk, pixels) == 0x10) ? 1 : -1];
typedef char CansCcbChunkCcbOffsetCheck[
    (offsetof(CansCcbChunk, ccb) == 0x0C) ? 1 : -1];
typedef char CansCcbChunkSizeCheck[
    (sizeof(CansCcbChunk) == 0x50) ? 1 : -1];
typedef char CansPlutChunkSizeCheck[
    (sizeof(CansPlutChunk) == 0x10) ? 1 : -1];
typedef char CansAaclChunkPayloadOffsetCheck[
    (offsetof(CansAaclChunk, payload) == 0x10) ? 1 : -1];
typedef char CansOffsetAnimationSizeCheck[
    (sizeof(CansOffsetAnimation) == 0x18) ? 1 : -1];
typedef char CansAnimationMetadataSizeCheck[
    (sizeof(CansAnimationMetadata) == 0x1C) ? 1 : -1];
typedef char CansAnimationMetadataCcbOffsetCheck[
    (offsetof(CansAnimationMetadata, ccb) == 0x00) ? 1 : -1];
typedef char CansAnimationMetadataPlutOffsetCheck[
    (offsetof(CansAnimationMetadata, plut) == 0x04) ? 1 : -1];
typedef char CansAnimationMetadataPixelDataOffsetCheck[
    (offsetof(CansAnimationMetadata, pixel_data) == 0x08) ? 1 : -1];
typedef char CansAnimationMetadataCenterXOffsetCheck[
    (offsetof(CansAnimationMetadata, center_x) == 0x0C) ? 1 : -1];
typedef char CansAnimationMetadataCenterYOffsetCheck[
    (offsetof(CansAnimationMetadata, center_y) == 0x10) ? 1 : -1];
typedef char CansAnimationMetadataHotspotCountOffsetCheck[
    (offsetof(CansAnimationMetadata, hotspot_count) == 0x14) ? 1 : -1];
typedef char CansAnimationMetadataHotspotsOffsetCheck[
    (offsetof(CansAnimationMetadata, hotspots) == 0x18) ? 1 : -1];
typedef char FrontEndCelResourceInfoSizeCheck[
    (sizeof(FrontEndCelResourceInfo) == 0x24) ? 1 : -1];
typedef char CcbNextOffsetCheck[
    (offsetof(CCB, ccb_NextPtr) == 0x04) ? 1 : -1];
typedef char CcbSourceOffsetCheck[
    (offsetof(CCB, ccb_SourcePtr) == 0x08) ? 1 : -1];
typedef char CcbPlutOffsetCheck[
    (offsetof(CCB, ccb_PLUTPtr) == 0x0C) ? 1 : -1];
typedef char CcbWidthOffsetCheck[
    (offsetof(CCB, ccb_Width) == 0x3C) ? 1 : -1];
typedef char CcbHeightOffsetCheck[
    (offsetof(CCB, ccb_Height) == 0x40) ? 1 : -1];
typedef char CcbSizeCheck[
    (sizeof(CCB) == 0x44) ? 1 : -1];

typedef enum FamilyCansAnimationExtentStatus
{
  FAMILY_CANS_ANIMATION_EXTENT_STALE = -1,
  FAMILY_CANS_ANIMATION_EXTENT_UNBOUNDED = 0,
  FAMILY_CANS_ANIMATION_EXTENT_BOUNDED = 1
} FamilyCansAnimationExtentStatus;

struct FrontEndCelDescriptor;

int
apply_front_end_cel_descriptors(const struct FrontEndCelDescriptor *descriptors);
void
parse_cans_animation_metadata(CansAnimationData     *animation,
                              CansAnimationMetadata *metadata);
int
family_cans_animation_has_valid_root(unsigned int             selector,
                                     const CansAnimationData *animation);
int
family_cans_animation_loaded_extent(const CansAnimationData *animation,
                                    const void             **loaded_end,
                                    unsigned int            *group_index);
CCB *
parse_cans_cel_chunk_stream(CansChunkHeader *first_chunk,
                            void           **out_plut);
void
resolve_ccb_dimension_preambles(CCB *ccb);
/* Metadata-frame requests write only the 0x1C-byte metadata prefix. */
void
CANS_GetAnimFrame(CansAnimationData        *animation,
                  int                       frame,
                  CansAnimationFrameOutput *output);
void
CANS_NewGetAnimFrame(CansOffsetAnimation      *animation,
                     int                       frame,
                     CansAnimationFrameOutput *output);
int
visit_family_resource_animation_frame(
    int                       object_ref,
    int                       resource_kind,
    int                       frame,
    CansAnimationFrameVisitor visitor,
    void                     *context);
int
CANS_RelocateAnimationOffsets(CansOffsetAnimation *animation);
int
load_front_end_cel_resource(int *descriptor_words,
                            int *resource_info_words);
int
draw_front_end_cel(int resource_id);
int
finalize_front_end_cel_resource(int resource_id,
                                int release_mode);

#endif
