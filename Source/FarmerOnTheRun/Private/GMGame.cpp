// Fill out your copyright notice in the Description page of Project Settings.


#include "GMGame.h"
#include "GamePlayerController.h"
#include "Kismet/GameplayStatics.h"

AGMGame::AGMGame()
{
	PlayerControllerClass = nullptr;
	DefaultPawnClass = nullptr;
}

