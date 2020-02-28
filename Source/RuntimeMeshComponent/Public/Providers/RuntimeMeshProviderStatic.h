// Copyright 2016-2019 Chris Conway (Koderz). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshProvider.h"
#include "RuntimeMeshProviderStatic.generated.h"

UCLASS(HideCategories = Object, BlueprintType)
class RUNTIMEMESHCOMPONENT_API URuntimeMeshProviderStatic : public URuntimeMeshProvider
{
	GENERATED_BODY()

private:
	FORCEINLINE int64 GenerateKeyFromLODAndSection(int32 LODIndex, int32 SectionId)
	{
		return (((int64)LODIndex) << 32) | ((int64)SectionId);
	}
private:
	TMap<int64, TTuple<FRuntimeMeshRenderableMeshData, FBoxSphereBounds>> StoredMeshData;
	
	int32 LODForMeshCollision;
	TSet<int32> SectionsForMeshCollision;

	FRuntimeMeshCollisionSettings CollisionSettings;
	TOptional<FRuntimeMeshCollisionData> CollisionMesh;
	FBoxSphereBounds CombinedBounds;
public:

	URuntimeMeshProviderStatic();

	
	// This brings forward the internal CreateSection as there's nothing wrong with using that version
	using URuntimeMeshProvider::CreateSection;

	UFUNCTION(BlueprintCallable, Category = "RuntimeMesh|Providers|Static", Meta=(DisplayName = "Create Section"))
	void CreateSection_Blueprint(int32 LODIndex, int32 SectionId, const FRuntimeMeshSectionProperties& SectionProperties, const FRuntimeMeshRenderableMeshData& SectionData)
	{
		URuntimeMeshProvider::CreateSection(LODIndex, SectionId, SectionProperties);
		UpdateSectionInternal(LODIndex, SectionId, SectionData, GetBoundsFromMeshData(SectionData));
	}

	void CreateSection(int32 LODIndex, int32 SectionId, const FRuntimeMeshSectionProperties& SectionProperties, const FRuntimeMeshRenderableMeshData& SectionData)
	{
		URuntimeMeshProvider::CreateSection(LODIndex, SectionId, SectionProperties);
		UpdateSectionInternal(LODIndex, SectionId, SectionData, GetBoundsFromMeshData(SectionData));
	}
	void CreateSection(int32 LODIndex, int32 SectionId, const FRuntimeMeshSectionProperties& SectionProperties, FRuntimeMeshRenderableMeshData&& SectionData)
	{
		URuntimeMeshProvider::CreateSection(LODIndex, SectionId, SectionProperties);
		UpdateSectionInternal(LODIndex, SectionId, MoveTemp(SectionData), GetBoundsFromMeshData(SectionData));
	}
	void CreateSection(int32 LODIndex, int32 SectionId, const FRuntimeMeshSectionProperties& SectionProperties, const FRuntimeMeshRenderableMeshData& SectionData, FBoxSphereBounds KnownBounds)
	{
		URuntimeMeshProvider::CreateSection(LODIndex, SectionId, SectionProperties);
		UpdateSectionInternal(LODIndex, SectionId, SectionData, GetBoundsFromMeshData(SectionData));
	}
	void CreateSection(int32 LODIndex, int32 SectionId, const FRuntimeMeshSectionProperties& SectionProperties, FRuntimeMeshRenderableMeshData&& SectionData, FBoxSphereBounds KnownBounds)
	{
		URuntimeMeshProvider::CreateSection(LODIndex, SectionId, SectionProperties);
		UpdateSectionInternal(LODIndex, SectionId, MoveTemp(SectionData), GetBoundsFromMeshData(SectionData));
	}

	void UpdateSection(int32 LODIndex, int32 SectionId, const FRuntimeMeshRenderableMeshData& SectionData)
	{
		UpdateSectionInternal(LODIndex, SectionId, SectionData, GetBoundsFromMeshData(SectionData));
	}
	void UpdateSection(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData&& SectionData)
	{
		UpdateSectionInternal(LODIndex, SectionId, MoveTemp(SectionData), GetBoundsFromMeshData(SectionData));
	}
	void UpdateSection(int32 LODIndex, int32 SectionId, const FRuntimeMeshRenderableMeshData& SectionData, FBoxSphereBounds KnownBounds)
	{
		UpdateSectionInternal(LODIndex, SectionId, SectionData, KnownBounds);
	}
	void UpdateSection(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData&& SectionData, FBoxSphereBounds KnownBounds)
	{
		UpdateSectionInternal(LODIndex, SectionId, MoveTemp(SectionData), KnownBounds);
	}

	UFUNCTION(BlueprintCallable, Category = "RuntimeMesh|Providers|Static")
	void ClearSection(int32 LODIndex, int32 SectionId);


	UFUNCTION(BlueprintCallable, Category = "RuntimeMesh|Providers|Static")
	void SetCollisionSettings(const FRuntimeMeshCollisionSettings& NewCollisionSettings);

	void SetCollisionMesh(const FRuntimeMeshCollisionData& NewCollisionMesh);

	UFUNCTION(BlueprintCallable, Category = "RuntimeMesh|Providers|Static")
	void SetRenderableLODForCollision(int32 LODIndex);

	UFUNCTION(BlueprintCallable, Category = "RuntimeMesh|Providers|Static")
	void SetRenderableSectionAffectsCollision(int32 SectionId, bool bCollisionEnabled);

protected:
	virtual FBoxSphereBounds GetBounds_Implementation() override { return CombinedBounds; }

	virtual bool GetSectionMeshForLOD_Implementation(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData& MeshData) override;

	virtual FRuntimeMeshCollisionSettings GetCollisionSettings_Implementation() override;
	virtual bool HasCollisionMesh_Implementation() override;
	virtual bool GetCollisionMesh_Implementation(FRuntimeMeshCollisionData& CollisionData) override;

private:
	void UpdateBounds();
	FBoxSphereBounds GetBoundsFromMeshData(const FRuntimeMeshRenderableMeshData& MeshData);


	void UpdateSectionInternal(int32 LODIndex, int32 SectionId, const FRuntimeMeshRenderableMeshData& SectionData, FBoxSphereBounds KnownBounds)
	{
		FRuntimeMeshRenderableMeshData TempData = SectionData;
		UpdateSectionInternal(LODIndex, SectionId, MoveTemp(TempData), KnownBounds);
	}
	void UpdateSectionInternal(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData&& SectionData, FBoxSphereBounds KnownBounds)
	{
		SectionData.HasValidMeshData(true);

		TTuple<FRuntimeMeshRenderableMeshData, FBoxSphereBounds>& NewCache = StoredMeshData.FindOrAdd(GenerateKeyFromLODAndSection(LODIndex, SectionId));
		NewCache = MakeTuple<FRuntimeMeshRenderableMeshData, FBoxSphereBounds>(MoveTemp(SectionData), MoveTemp(KnownBounds));
		UpdateBounds();
		MarkSectionDirty(LODIndex, SectionId);
	}
};