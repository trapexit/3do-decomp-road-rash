#include "cans_animation_binding.h"
#include "front_end_cel_runtime.h"
#include "resource_manager_runtime.h"

#define RIDER_CANS_RESOURCE_TYPE 0x43414E53U
#define RIDER_ANIMATION_RESOURCE_TYPE 0x414E494DU

enum RiderAnimationResourceConstants
{
  RIDER_RESOURCE_PAIR_PLAYER = 0,
  RIDER_RESOURCE_PAIR_CHALLENGE = 1,
  RIDER_RESOURCE_PAIR_COUNT = 2,
  RIDER_CANS_RESOURCE_ID_OFFSET = 1,
  RIDER_PRIMARY_ANIMATION_RESOURCE_ID_OFFSET = 2,
  RIDER_SECONDARY_ANIMATION_RESOURCE_ID_OFFSET = 4,
  RIDER_ANIMATION_RESOURCE_LOAD_FLAGS = 8
};

enum RiderAnimationBindingCounts
{
  RIDER_BINDING_COUNT_ONE = 1,
  RIDER_BINDING_COUNT_TWO = 2,
  RIDER_BINDING_COUNT_THREE = 3,
  RIDER_BINDING_COUNT_FOUR = 4,
  RIDER_BINDING_COUNT_FIVE = 5,
  RIDER_BINDING_COUNT_SIX = 6,
  RIDER_BINDING_COUNT_SEVEN = 7,
  RIDER_BINDING_COUNT_EIGHT = 8
};

enum { RIDER_BINDING_LITERAL_REFERENCE_COUNT = 107 };

typedef struct RiderAnimationResourceReferencePool
{
  const RuntimeFeatureFlags *feature_flags;
  AnimationResourcePair *primary_resources;
  AnimationResourcePair *secondary_resources;
} RiderAnimationResourceReferencePool;

typedef union RiderAnimationBindingLiteralReference
{
  const AnimationBindingSpec *specs;
  CelAnimationBinding *bindings;
  const RuntimeFeatureFlags *feature_flags;
  const AnimationResourcePair *resources;
} RiderAnimationBindingLiteralReference;

typedef struct RiderAnimationBindingLiteralPool
{
  unsigned int cans_resource_type;
  unsigned int animation_resource_type;
  unsigned int offset_animation_tag;
  RiderAnimationBindingLiteralReference
    batch_operands[RIDER_BINDING_LITERAL_REFERENCE_COUNT];
} RiderAnimationBindingLiteralPool;

/* Starts of the exact resource-key batches in rider_animation_channels. */
enum RiderAnimationSpecOffset
{
  RIDER_SPEC_SML1 = 0,
  RIDER_SPEC_SML2 = 1,
  RIDER_SPEC_RIDE = 2,
  RIDER_SPEC_TUCK = 3,
  RIDER_SPEC_WHEE = 4,
  RIDER_SPEC_ARM = 5,
  RIDER_SPEC_ONE_ARM = 7,
  RIDER_SPEC_ARM_B = 9,
  RIDER_SPEC_STOP = 10,
  RIDER_SPEC_HIT = 11,
  RIDER_SPEC_CHL_UPPER = 13,
  RIDER_SPEC_CHR_UPPER = 18,
  RIDER_SPEC_CHL_LOWER = 23,
  RIDER_SPEC_CHR_LOWER = 28,
  RIDER_SPEC_SCL = 33,
  RIDER_SPEC_SCR = 35,
  RIDER_SPEC_CLL_UPPER = 37,
  RIDER_SPEC_CLR_UPPER = 40,
  RIDER_SPEC_CLL_LOWER = 43,
  RIDER_SPEC_CLR_LOWER = 46,
  RIDER_SPEC_PUL_UPPER = 53,
  RIDER_SPEC_PUR_UPPER = 56,
  RIDER_SPEC_PUL_LOWER = 59,
  RIDER_SPEC_PUR_LOWER = 62,
  RIDER_SPEC_KIL = 65,
  RIDER_SPEC_KIR = 67,
  RIDER_SPEC_FRONT_FLIP = 69,
  RIDER_SPEC_ROLL = 76,
  RIDER_SPEC_SRO = 84,
  RIDER_SPEC_SSP = 88,
  RIDER_SPEC_BSP = 92,
  RIDER_SPEC_FLY = 96,
  RIDER_SPEC_LAND = 101,
  RIDER_SPEC_FALL = 105,
  RIDER_SPEC_STAND = 107,
  RIDER_SPEC_SPARK = 112,
  RIDER_SPEC_RUN = 116,
  RIDER_SPEC_PUP_LEFT = 122,
  RIDER_SPEC_PUP_RIGHT = 127,
  RIDER_SPEC_MOUNT_LEFT = 132,
  RIDER_SPEC_MOUNT_RIGHT = 137,
  RIDER_SPEC_BFL = 142,
  RIDER_SPEC_BSP_LEFT = 147,
  RIDER_SPEC_BSP_RIGHT = 151,
  RIDER_SPEC_BFA_LEFT = 155,
  RIDER_SPEC_BFA_RIGHT = 158,
  RIDER_SPEC_BST_LEFT = 161,
  RIDER_SPEC_BST_RIGHT = 162
};

enum RiderPrimaryBindingOffset
{
  RIDER_PRIMARY_BINDING_SML1 = 0,
  RIDER_PRIMARY_BINDING_SML2 = 1,
  RIDER_PRIMARY_BINDING_RIDE = 2,
  RIDER_PRIMARY_BINDING_TUCK = 3,
  RIDER_PRIMARY_BINDING_WHEE = 4,
  RIDER_PRIMARY_BINDING_ARM = 5,
  RIDER_PRIMARY_BINDING_ONE_ARM = 7,
  RIDER_PRIMARY_BINDING_ARM_B = 9,
  RIDER_PRIMARY_BINDING_STOP = 10,
  RIDER_PRIMARY_BINDING_HIT = 11,
  RIDER_PRIMARY_BINDING_CHL_UPPER = 13,
  RIDER_PRIMARY_BINDING_CHR_UPPER = 18,
  RIDER_PRIMARY_BINDING_CHL_LOWER = 23,
  RIDER_PRIMARY_BINDING_CHR_LOWER = 28,
  RIDER_PRIMARY_BINDING_SCL = 33,
  RIDER_PRIMARY_BINDING_SCR = 35,
  RIDER_PRIMARY_BINDING_CLL_UPPER = 37,
  RIDER_PRIMARY_BINDING_CLR_UPPER = 40,
  RIDER_PRIMARY_BINDING_CLL_LOWER = 43,
  RIDER_PRIMARY_BINDING_CLR_LOWER = 46,
  RIDER_PRIMARY_BINDING_PUL_UPPER = 53,
  RIDER_PRIMARY_BINDING_PUR_UPPER = 56,
  RIDER_PRIMARY_BINDING_PUL_LOWER = 59,
  RIDER_PRIMARY_BINDING_PUR_LOWER = 62,
  RIDER_PRIMARY_BINDING_KIL = 65,
  RIDER_PRIMARY_BINDING_KIR = 67,
  RIDER_PRIMARY_BINDING_SRO = 69,
  RIDER_PRIMARY_BINDING_SSP = 73,
  RIDER_PRIMARY_BINDING_BSP = 77,
  RIDER_PRIMARY_BINDING_FLY = 81,
  RIDER_PRIMARY_BINDING_LAND = 86,
  RIDER_PRIMARY_BINDING_FALL = 90,
  RIDER_PRIMARY_BINDING_STAND = 92,
  RIDER_PRIMARY_BINDING_SPARK = 97,
  RIDER_PRIMARY_BINDING_PUP_LEFT = 101,
  RIDER_PRIMARY_BINDING_PUP_RIGHT = 106,
  RIDER_PRIMARY_BINDING_MOUNT_LEFT = 111,
  RIDER_PRIMARY_BINDING_MOUNT_RIGHT = 116,
  RIDER_PRIMARY_BINDING_BFL = 121,
  RIDER_PRIMARY_BINDING_BSP_LEFT = 126,
  RIDER_PRIMARY_BINDING_BSP_RIGHT = 130,
  RIDER_PRIMARY_BINDING_BFA_LEFT = 134,
  RIDER_PRIMARY_BINDING_BFA_RIGHT = 137,
  RIDER_PRIMARY_BINDING_BST_LEFT = 140,
  RIDER_PRIMARY_BINDING_BST_RIGHT = 141
};

enum RiderChallengeBindingOffset
{
  RIDER_CHALLENGE_BINDING_RIDE = 0,
  RIDER_CHALLENGE_BINDING_CLL_UPPER = 1,
  RIDER_CHALLENGE_BINDING_CLR_UPPER = 4,
  RIDER_CHALLENGE_BINDING_STOP = 7,
  RIDER_CHALLENGE_BINDING_HIT = 8,
  RIDER_CHALLENGE_BINDING_BFL = 10,
  RIDER_CHALLENGE_BINDING_FLY = 15,
  RIDER_CHALLENGE_BINDING_BST_LEFT = 20,
  RIDER_CHALLENGE_BINDING_BFA_LEFT = 21,
  RIDER_CHALLENGE_BINDING_SPARK = 24
};

MemoryHandle *
RSRC_LoadResource(int resource_type,
                  int resource_id,
                  int flags);

int
load_rider_animation_resources(void)
{
  AnimationResourcePair *primary_resources;
  AnimationResourcePair *secondary_resources;
  const AnimationResourcePair *challenge_resources;
  MemoryHandle *resource_handle;
  signed char resource_pair_index;

  primary_resources = primary_animation_resource_pairs;
  secondary_resources = secondary_animation_resource_pairs;

  for(resource_pair_index = RIDER_RESOURCE_PAIR_PLAYER;
      resource_pair_index < RIDER_RESOURCE_PAIR_COUNT;
      resource_pair_index++)
    {
      if(primary_resources[resource_pair_index].cans_resource == 0)
        {
          if(resource_pair_index == RIDER_RESOURCE_PAIR_CHALLENGE &&
             runtime_feature_flags.unused_slot_3.value == 0)
            {
              primary_resources[resource_pair_index].animation_resource = 0;
              secondary_resources[resource_pair_index].cans_resource = 0;
              secondary_resources[resource_pair_index].animation_resource =
                0;
            }
          else
            {
              resource_handle = RSRC_LoadResource(
                RIDER_CANS_RESOURCE_TYPE,
                resource_pair_index + RIDER_CANS_RESOURCE_ID_OFFSET,
                RIDER_ANIMATION_RESOURCE_LOAD_FLAGS);
              primary_resources[resource_pair_index].cans_resource =
                resource_handle->data;

              resource_handle = RSRC_LoadResource(
                RIDER_ANIMATION_RESOURCE_TYPE,
                resource_pair_index +
                RIDER_PRIMARY_ANIMATION_RESOURCE_ID_OFFSET,
                RIDER_ANIMATION_RESOURCE_LOAD_FLAGS);
              primary_resources[resource_pair_index].animation_resource =
                resource_handle->data;

              secondary_resources[resource_pair_index].cans_resource =
                primary_resources[resource_pair_index].cans_resource;
              resource_handle = RSRC_LoadResource(
                RIDER_ANIMATION_RESOURCE_TYPE,
                resource_pair_index +
                RIDER_SECONDARY_ANIMATION_RESOURCE_ID_OFFSET,
                RIDER_ANIMATION_RESOURCE_LOAD_FLAGS);
              secondary_resources[resource_pair_index].animation_resource =
                resource_handle->data;

              if(*(unsigned int *)
                 primary_resources[resource_pair_index].animation_resource ==
                 CANS_CHUNK_TAG_OFSS)
                {
                  CANS_RelocateAnimationOffsets(
                    (CansOffsetAnimation *)
                    primary_resources[resource_pair_index].animation_resource);
                }
              if(*(unsigned int *)
                 secondary_resources[resource_pair_index].animation_resource ==
                 CANS_CHUNK_TAG_OFSS)
                {
                  CANS_RelocateAnimationOffsets(
                    (CansOffsetAnimation *)
                    secondary_resources[resource_pair_index].animation_resource);
                }
            }
        }
    }

  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_SML1],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_SML1],
    RIDER_BINDING_COUNT_ONE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_SML2],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_SML2],
    RIDER_BINDING_COUNT_ONE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_RIDE],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_RIDE],
    RIDER_BINDING_COUNT_ONE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_TUCK],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_TUCK],
    RIDER_BINDING_COUNT_ONE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_WHEE],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_WHEE],
    RIDER_BINDING_COUNT_ONE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_ARM],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_ARM],
    RIDER_BINDING_COUNT_TWO);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_ONE_ARM],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_ONE_ARM],
    RIDER_BINDING_COUNT_TWO);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_ARM_B],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_ARM_B],
    RIDER_BINDING_COUNT_ONE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_STOP],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_STOP],
    RIDER_BINDING_COUNT_ONE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_HIT],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_HIT],
    RIDER_BINDING_COUNT_TWO);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_CHL_UPPER],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_CHL_UPPER],
    RIDER_BINDING_COUNT_FIVE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_CHR_UPPER],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_CHR_UPPER],
    RIDER_BINDING_COUNT_FIVE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_CHL_LOWER],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_CHL_LOWER],
    RIDER_BINDING_COUNT_FIVE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_CHR_LOWER],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_CHR_LOWER],
    RIDER_BINDING_COUNT_FIVE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_SCL],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_SCL],
    RIDER_BINDING_COUNT_TWO);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_SCR],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_SCR],
    RIDER_BINDING_COUNT_TWO);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_CLL_UPPER],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_CLL_UPPER],
    RIDER_BINDING_COUNT_THREE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_CLR_UPPER],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_CLR_UPPER],
    RIDER_BINDING_COUNT_THREE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_CLL_LOWER],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_CLL_LOWER],
    RIDER_BINDING_COUNT_THREE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_CLR_LOWER],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_CLR_LOWER],
    RIDER_BINDING_COUNT_THREE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_PUL_UPPER],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_PUL_UPPER],
    RIDER_BINDING_COUNT_THREE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_PUR_UPPER],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_PUR_UPPER],
    RIDER_BINDING_COUNT_THREE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_PUL_LOWER],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_PUL_LOWER],
    RIDER_BINDING_COUNT_THREE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_PUR_LOWER],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_PUR_LOWER],
    RIDER_BINDING_COUNT_THREE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_KIL],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_KIL],
    RIDER_BINDING_COUNT_TWO);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_KIR],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_KIR],
    RIDER_BINDING_COUNT_TWO);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_FRONT_FLIP],
    gPlayerOneFrontFlipBindings, RIDER_BINDING_COUNT_SEVEN);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_ROLL],
    gPlayerOneRollBindings, RIDER_BINDING_COUNT_EIGHT);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_SRO],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_SRO],
    RIDER_BINDING_COUNT_FOUR);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_SSP],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_SSP],
    RIDER_BINDING_COUNT_FOUR);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_BSP],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_BSP],
    RIDER_BINDING_COUNT_FOUR);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_FLY],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_FLY],
    RIDER_BINDING_COUNT_FIVE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_LAND],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_LAND],
    RIDER_BINDING_COUNT_FOUR);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_FALL],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_FALL],
    RIDER_BINDING_COUNT_TWO);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_STAND],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_STAND],
    RIDER_BINDING_COUNT_FIVE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_SPARK],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_SPARK],
    RIDER_BINDING_COUNT_FOUR);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_RUN],
    gPlayerOneRunBindings, RIDER_BINDING_COUNT_SIX);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_PUP_LEFT],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_PUP_LEFT],
    RIDER_BINDING_COUNT_FIVE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_PUP_RIGHT],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_PUP_RIGHT],
    RIDER_BINDING_COUNT_FIVE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_MOUNT_LEFT],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_MOUNT_LEFT],
    RIDER_BINDING_COUNT_FIVE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_MOUNT_RIGHT],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_MOUNT_RIGHT],
    RIDER_BINDING_COUNT_FIVE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_BFL],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_BFL],
    RIDER_BINDING_COUNT_FIVE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_BSP_LEFT],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_BSP_LEFT],
    RIDER_BINDING_COUNT_FOUR);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_BSP_RIGHT],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_BSP_RIGHT],
    RIDER_BINDING_COUNT_FOUR);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_BFA_LEFT],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_BFA_LEFT],
    RIDER_BINDING_COUNT_THREE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_BFA_RIGHT],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_BFA_RIGHT],
    RIDER_BINDING_COUNT_THREE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_BST_LEFT],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_BST_LEFT],
    RIDER_BINDING_COUNT_ONE);
  resolve_cans_animation_bindings(
    primary_resources, &rider_animation_channels[RIDER_SPEC_BST_RIGHT],
    &rider_one_animation_bindings[RIDER_PRIMARY_BINDING_BST_RIGHT],
    RIDER_BINDING_COUNT_ONE);

  if(runtime_feature_flags.unused_slot_3.value == 0)
    {
      return 0;
    }

  challenge_resources =
    &primary_resources[RIDER_RESOURCE_PAIR_CHALLENGE];
  resolve_cans_animation_bindings(
    challenge_resources, &rider_animation_channels[RIDER_SPEC_RIDE],
    &rider_two_animation_bindings[RIDER_CHALLENGE_BINDING_RIDE],
    RIDER_BINDING_COUNT_ONE);
  resolve_cans_animation_bindings(
    challenge_resources, &rider_animation_channels[RIDER_SPEC_STOP],
    &rider_two_animation_bindings[RIDER_CHALLENGE_BINDING_STOP],
    RIDER_BINDING_COUNT_ONE);
  resolve_cans_animation_bindings(
    challenge_resources, &rider_animation_channels[RIDER_SPEC_HIT],
    &rider_two_animation_bindings[RIDER_CHALLENGE_BINDING_HIT],
    RIDER_BINDING_COUNT_TWO);
  resolve_cans_animation_bindings(
    challenge_resources,
    &rider_animation_channels[RIDER_SPEC_CLL_UPPER],
    &rider_two_animation_bindings[RIDER_CHALLENGE_BINDING_CLL_UPPER],
    RIDER_BINDING_COUNT_THREE);
  resolve_cans_animation_bindings(
    challenge_resources,
    &rider_animation_channels[RIDER_SPEC_CLR_UPPER],
    &rider_two_animation_bindings[RIDER_CHALLENGE_BINDING_CLR_UPPER],
    RIDER_BINDING_COUNT_THREE);
  resolve_cans_animation_bindings(
    challenge_resources,
    &rider_animation_channels[RIDER_SPEC_FRONT_FLIP],
    gPlayerTwoFrontFlipBindings, RIDER_BINDING_COUNT_SEVEN);
  resolve_cans_animation_bindings(
    challenge_resources, &rider_animation_channels[RIDER_SPEC_FLY],
    &rider_two_animation_bindings[RIDER_CHALLENGE_BINDING_FLY],
    RIDER_BINDING_COUNT_FIVE);
  resolve_cans_animation_bindings(
    challenge_resources, &rider_animation_channels[RIDER_SPEC_BFL],
    &rider_two_animation_bindings[RIDER_CHALLENGE_BINDING_BFL],
    RIDER_BINDING_COUNT_FIVE);
  resolve_cans_animation_bindings(
    challenge_resources, &rider_animation_channels[RIDER_SPEC_BST_LEFT],
    &rider_two_animation_bindings[RIDER_CHALLENGE_BINDING_BST_LEFT],
    RIDER_BINDING_COUNT_ONE);
  resolve_cans_animation_bindings(
    challenge_resources, &rider_animation_channels[RIDER_SPEC_BFA_LEFT],
    &rider_two_animation_bindings[RIDER_CHALLENGE_BINDING_BFA_LEFT],
    RIDER_BINDING_COUNT_THREE);
  return resolve_cans_animation_bindings(
    challenge_resources, &rider_animation_channels[RIDER_SPEC_SPARK],
    &rider_two_animation_bindings[RIDER_CHALLENGE_BINDING_SPARK],
    RIDER_BINDING_COUNT_FOUR);
}
