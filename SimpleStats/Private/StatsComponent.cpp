// Fill out your copyright notice in the Description page of Project Settings.

#include "StatsComponent.h"

// Sets default values for this component's properties
UStatsComponent::UStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	InitStats();
}

void UStatsComponent::InitStats()
{
	if ( StatsDataTable )
	{
		TArray<FName> RowNames = StatsDataTable->GetRowNames();
		FString ContextString = "";

		for ( FName RowName : RowNames )
		{
			FStatData* RowData = StatsDataTable->FindRow<FStatData>( RowName, ContextString );
			FStatData Data = *RowData; // Initialize Data from Row struct
			Data.SetCurrentValue( Data.GetBaseValue() ); // Init Current value to Base value
			Stats.Add( RowName, Data );
		}
	}
}

FStatData& UStatsComponent::GetStat( FName StatName )
{
	return *Stats.Find( StatName );
}

void UStatsComponent::StatTick()
{
	TArray<FName> StatNames;
	Stats.GenerateKeyArray( StatNames );
	
	for ( FName Name : StatNames )
	{
		GetStat( Name ).StatTickUpdate();
	}
}

void UStatsComponent::PrintStats()
{
	TArray<FName> StatNames;
	Stats.GenerateKeyArray( StatNames );

	for ( FName Name : StatNames )
	{
		if ( GEngine )
			GEngine->AddOnScreenDebugMessage( -1, 0.0f, FColor::Yellow, FString::Printf( TEXT( "%s : %f" ), *Name.ToString(), GetStatCurrentValue(Name)));
	}
}

float UStatsComponent::GetStatCurrentValue( FName StatName )
{
	FStatData& Stat = GetStat( StatName );

	if ( &Stat )
		return Stat.GetCurrentValue();

	// Stat not found
	return 0.0f;
}

float UStatsComponent::SetStatCurrentValue( FName StatName, float Value )
{
	FStatData& Stat = GetStat( StatName );

	if ( &Stat )
	{
		float NewValue = Stat.SetCurrentValue( Value );
		OnStatChanged.Broadcast( StatName, NewValue );
		return NewValue;
	}
		
	// Stat not found
	return 0.0f;
}

float UStatsComponent::AddStatCurrentValue( FName StatName, float Value )
{
	FStatData& Stat = GetStat( StatName );

	if ( &Stat )
		return Stat.AddCurrentValue( Value );

	// Stat not found
	return 0.0f;
}

float UStatsComponent::GetStatBaseValue( FName StatName ) 
{
	FStatData& Stat = GetStat( StatName );

	if ( &Stat )
		return Stat.GetBaseValue();

	// Stat not found
	return 0.0f;
}

float UStatsComponent::SetStatBaseValue( FName StatName, float Value )
{
	FStatData& Stat = GetStat( StatName );

	if ( &Stat )
		return Stat.SetBaseValue(Value);

	// Stat not found
	return 0.0f;
}

float UStatsComponent::GetStatChangePerTickValue( FName StatName ) 
{
	FStatData& Stat = GetStat( StatName );

	if ( &Stat )
		return Stat.GetChangePerTick();

	// Stat not found
	return 0.0f;
}

bool UStatsComponent::IsStatChangingPerTick( FName StatName ) 
{
	FStatData& Stat = GetStat( StatName );

	if ( &Stat )
		return Stat.IsChangingPerTick();

	// Stat not found
	return false;
}

float UStatsComponent::SetStatChangePerTickValue( FName StatName, float Value )
{
	FStatData& Stat = GetStat( StatName );

	if ( &Stat )
		return Stat.SetChangePerTickValue(Value);

	// Stat not found
	return 0.f;
}

float UStatsComponent::GetStatMinValue( FName StatName ) 
{
	FStatData& Stat = GetStat( StatName );

	if ( &Stat )
		return Stat.GetMinValue();

	// Stat not found
	return 0.0f;
}

float UStatsComponent::SetStatMinValue( FName StatName, float Value )
{
	FStatData& Stat = GetStat( StatName );

	if ( &Stat )
		return Stat.SetMinValue(Value);

	// Stat not found
	return 0.0f;
}

float UStatsComponent::GetStatMaxValue( FName StatName ) 
{
	FStatData& Stat = GetStat( StatName );

	if ( &Stat )
		return Stat.GetMaxValue();

	// Stat not found
	return 0.0f;
}

float UStatsComponent::SetStatMaxValue( FName StatName, float Value )
{
	FStatData& Stat = GetStat( StatName );

	if ( &Stat )
		return Stat.SetMaxValue( Value );

	// Stat not found
	return 0.0f;
}

float UStatsComponent::ResetStat( FName StatName )
{
	FStatData& Stat = GetStat( StatName );

	if ( &Stat )
		return Stat.ResetStat();

	// Stat not found
	return 0.0f;
}

void UStatsComponent::ResetAllStats()
{
	TArray<FName> StatNames;
	Stats.GenerateKeyArray( StatNames );

	for ( FName Name : StatNames )
	{
		ResetStat( Name );
	}
}

// Called every frame
void UStatsComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if ( bPrintStats )
	{
		PrintStats();
	}
}