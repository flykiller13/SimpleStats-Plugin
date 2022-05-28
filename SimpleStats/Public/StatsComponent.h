// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Curves/CurveFloat.h"
#include "StatsComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN( LogMyStats, Display, All );

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FStatValueChanged, FName, Name, float, Value );

USTRUCT( BlueprintType )
struct SIMPLESTATS_API FStatData : public  FTableRowBase
{
	GENERATED_USTRUCT_BODY()

		FStatData()
		: BaseValue( 0.f ), MinValue( 0.f ), MaxValue( 100.f ), CurrentValue( 0.f ),  ChangePerTick( 0.f )
	{
	}

	FStatData( float InBaseValue, float InMinValue, float InMaxValue, float InChangePerTick )
		: BaseValue( InBaseValue ), MinValue( InMinValue ), MaxValue( InMaxValue ), CurrentValue( InBaseValue ),  ChangePerTick( InChangePerTick )
	{
	}

	virtual ~FStatData()
	{}

protected:

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Stat" )
		float BaseValue;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Stat" )
		float MinValue;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Stat" )
		float MaxValue;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Stat", meta = (EditCondition = "false") )
		float CurrentValue;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Stat" )
		float ChangePerTick;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Stat" )
		UCurveFloat* StatCurve;

public:

	/** Returns the current value, which includes temporary buffs */
	virtual float GetCurrentValue() const
	{
		return this->CurrentValue;
	}

	/** Modifies current value, normally only called by ability system or during initialization */
	virtual float SetCurrentValue( float NewValue )
	{
		this->CurrentValue = FMath::Clamp( NewValue, MinValue, MaxValue );
		return this->CurrentValue;
	}

	virtual float AddCurrentValue( float Value )
	{
		return SetCurrentValue( CurrentValue + Value );
	}

	/** Returns the base value which only includes permanent changes */
	virtual float GetBaseValue() const
	{
		return this->BaseValue;
	}

	/** Modifies the permanent base value, normally only called by ability system or during initialization */
	virtual float SetBaseValue( float NewValue )
	{
		this->BaseValue = FMath::Clamp( NewValue, MinValue, MaxValue );
		return this->BaseValue;
	}

	virtual float GetMinValue() const
	{
		return this->MinValue;
	}

	virtual float SetMinValue( float NewMinValue )
	{
		this->MinValue = NewMinValue;
		SetCurrentValue( this->CurrentValue ); // Update current value to match new restrictions
		return this->MinValue;
	}

	virtual float GetMaxValue() const
	{
		return this->MaxValue;
	}

	virtual float SetMaxValue( float NewMaxValue )
	{
		this->MaxValue = NewMaxValue;
		SetCurrentValue( this->CurrentValue ); // Update current value to match new restrictions
		return this->MaxValue;
	}

	virtual float GetChangePerTick() const
	{
		return this->ChangePerTick;
	}

	/** Returns true if stat should update by tick */
	virtual bool IsChangingPerTick()
	{
		return (GetChangePerTick() != 0.f);
	}

	virtual float SetChangePerTickValue( float NewValue )
	{
		this->ChangePerTick = NewValue;
		return this->ChangePerTick;
	}

	virtual void StatTickUpdate()
	{
		if ( IsChangingPerTick() )
		{
			AddCurrentValue( this->ChangePerTick );
		}
	}

	virtual float ResetStat()
	{
		return SetCurrentValue( this->BaseValue );
	}
};

UCLASS( Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent) )
class SIMPLESTATS_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStatsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION( BlueprintCallable )
		void InitStats();

public: 

	/*
	*  Properties and Delegates
	*/

	UPROPERTY( BlueprintReadOnly )
		TMap<FName, FStatData> Stats;

	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly )
		UDataTable* StatsDataTable;

	/*
	*  Main Stat Data struct control
	*/

	UFUNCTION( BlueprintCallable , BlueprintPure)
		FStatData& GetStat( FName StatName );

	/*
	*  Stat value control
	*/

	UFUNCTION( BlueprintCallable )
		void StatTick();

	UFUNCTION( BlueprintCallable )
		void PrintStats();

	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly )
		bool bPrintStats;

	// Current Value

	UFUNCTION( BlueprintCallable, BlueprintPure )
		float GetStatCurrentValue( FName StatName );

	UFUNCTION( BlueprintCallable)
		float SetStatCurrentValue( FName StatName, float Value );

	UFUNCTION( BlueprintCallable )
		float AddStatCurrentValue( FName StatName, float Value );

	// Base Value

	UFUNCTION( BlueprintCallable, BlueprintPure )
		float GetStatBaseValue( FName StatName );

	UFUNCTION( BlueprintCallable )
		float SetStatBaseValue( FName StatName, float Value );

	// Change Per Tick

	UFUNCTION( BlueprintCallable, BlueprintPure )
		float GetStatChangePerTickValue( FName StatName );

	UFUNCTION( BlueprintCallable )
		bool IsStatChangingPerTick( FName StatName );

	UFUNCTION( BlueprintCallable )
		float SetStatChangePerTickValue( FName StatName, float Value );

	// Min / Max

	UFUNCTION( BlueprintCallable, BlueprintPure )
		float GetStatMinValue( FName StatName );

	UFUNCTION( BlueprintCallable )
		float SetStatMinValue( FName StatName, float Value );

	UFUNCTION( BlueprintCallable, BlueprintPure )
		float GetStatMaxValue( FName StatName );

	UFUNCTION( BlueprintCallable )
		float SetStatMaxValue( FName StatName, float Value );

	// Reset

	UFUNCTION( BlueprintCallable )
		float ResetStat( FName StatName );

	UFUNCTION( BlueprintCallable )
		void ResetAllStats();

	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	/* Delegate - Called when a morph's value is changed */
	UPROPERTY( BlueprintAssignable )
		FStatValueChanged OnStatChanged;
};
