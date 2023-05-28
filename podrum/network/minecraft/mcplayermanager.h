#ifndef PODRUM_NETWORK_MINECRAFT_MCPLAYERMANAGER_H
#define PODRUM_NETWORK_MINECRAFT_MCPLAYERMANAGER_H

#include <podrum/debug.h>
#include <stdint.h>
#include <stdlib.h>
#include <podrum/network/raknet/rakmisc.h>

typedef struct {
	int64_t entity_id;
	misc_address_t address;
	int32_t protocol;
	char *display_name;
	char *identity;
	char *title_id;
	char *xuid;
	float x;
	float y;
	float z;
	float pitch;
	float yaw;
	int32_t gamemode;
	int32_t view_distance;
	uint8_t spawned;
} minecraft_player_t;

typedef struct {
	// key AnimatedImageData;
	char *ArmSize;
	char *CapeData;
	char *CapeId;
	int32_t CapeImageHeight;
	int32_t CapeImageWidth;
	uint8_t CapeOnClassicSkin;
	int32_t ClientRandomId;
	uint8_t CompatibleWithClientSideChunkGen;
	int32_t CurrentInputMode;
	int32_t DefaultInputMode;
	char *DeviceId;
	char *DeviceModel;
	int32_t DeviceOS;
	char *GameVersion;
	int32_t GuiScale;
	uint8_t IsEditorMode;
	char *LanguageCode;
	uint8_t OverrideSkin;
	// key PersonaPieces;
	uint8_t PersonaSkin;
	// key PieceTintColors;
	char *PlatformOfflineId;
	char *PlatformOnlineId;
    char *PlatformUserId;
	char *PlayFabId;
	uint8_t PremiumSkin;
	char *SelfSignedId;
	char *ServerAddress;
	char *SkinAnimationData;
	char *SkinColor;
	char *SkinData;
	char *SkinGeometryData;
	char *SkinGeometryDataEngineVersion;
	char *SkinId;
	int32_t SkinImageHeight;
	int32_t SkinImageWidth;
	char *SkinResourcePatch;
	char *ThirdPartyName;
	uint8_t ThirdPartyNameOnly;
	uint8_t TrustedSkin;
	int32_t UIProfile;
} minecraft_client_data_t;

typedef struct {
	minecraft_player_t *players;
	size_t size;
} minecraft_player_manager_t;

uint8_t has_minecraft_player_address(misc_address_t address, minecraft_player_manager_t *player_manager);

void add_minecraft_player(minecraft_player_t player, minecraft_player_manager_t *player_manager);

void remove_minecraft_player(misc_address_t address, minecraft_player_manager_t *player_manager);

minecraft_player_t *get_minecraft_player_address(misc_address_t address, minecraft_player_manager_t *player_manager);

uint8_t has_minecraft_player_display_name(char *display_name, minecraft_player_manager_t *player_manager);

minecraft_player_t *get_minecraft_player_display_name(char *display_name, minecraft_player_manager_t *player_manager);

uint8_t has_minecraft_player_entity_id(int64_t entity_id, minecraft_player_manager_t *player_manager);

#endif