#include "PolygonalMapGeneratorPrivatePCH.h"
#include "Misc/Char.h"
#include <string>
#include "ProceduralNameGenerator.h"

uint8 NameTileGenerator::Order = 0;
TMap<FString, TArray<FMarkovCharacter>> NameTileGenerator::Chains = TMap<FString, TArray<FMarkovCharacter>>();
TSet<FString> NameTileGenerator::WordPatterns = TSet<FString>();

bool NameTileGenerator::IsVowel(TCHAR ch)
{
	FString vowels = FString(TEXT("aeiou"));
	int32 index;
	return vowels.FindChar(ch, index);
}

FString NameTileGenerator::GetWordPattern(FString word)
{
	std::string pattern = "";

	TArray<TCHAR> wordArray = word.GetCharArray();
	for(int i = 0; i < wordArray.Num(); i++)
	{
		TCHAR ch = wordArray[i];
		pattern += IsVowel(ch) ? "v" : "c";
	}

	return FString(pattern.c_str());
}

void NameTileGenerator::IdentifyWordPattern(FString word)
{
	WordPatterns.Add(GetWordPattern(word));
}

void NameTileGenerator::AddCharacter(FString key, TCHAR ch)
{
	TArray<FMarkovCharacter> chain;

	if (Chains.Contains(key))
	{
		chain = Chains[key];
	}
	else
	{
		Chains.Add(key, chain);
	}

	bool bFound = false;
	for (int i = 0; i < chain.Num(); i++)
	{
		if (chain[i].Letter == ch)
		{
			chain[i].Instances++;
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		FMarkovCharacter letter = FMarkovCharacter();
		letter.Letter = ch;
		letter.Instances = 1;
		chain.Add(letter);
	}
	
	Chains[key] = chain;
}

void NameTileGenerator::ProcessWord(FString word)
{
	std::string wordStr = "";
	for (int i = 0; i < Order; i++)
	{
		wordStr += '_';
	}
	for (int i = 0; i < word.Len(); i++)
	{
		wordStr += word[i];
	}
	wordStr += '_';

	for (int i = 0; i < wordStr.size() - Order; i++)
	{
		std::string key = wordStr.substr(i, Order);
		FString unrealKey = FString(key.c_str());
		AddCharacter(unrealKey, wordStr[i + Order]);
	}
}

void NameTileGenerator::AnalyzeWord(FString word)
{
	IdentifyWordPattern(word);
	ProcessWord(word);
}

void NameTileGenerator::CalculateProbability()
{
	TMap<FString, TArray<FMarkovCharacter>> newChains;

	for (TPair<FString, TArray<FMarkovCharacter>>& kvp : Chains)
	{
		if(kvp.Key.IsEmpty() || kvp.Value.Num() == 0)
		{
			continue;
		}
		float totalInstances = 0.0f;
		TArray<FMarkovCharacter> characterArray = kvp.Value;
		for (int i = 0; i < characterArray.Num(); i++)
		{
			totalInstances += characterArray[i].Instances;
		}
		for (int i = 0; i < characterArray.Num(); i++)
		{
			characterArray[i].Probability = characterArray[i].Instances / totalInstances;
		}
		characterArray.HeapSort();
		newChains.Add(kvp.Key, characterArray);
	}

	Chains.Empty();
	Chains = newChains;
}

void NameTileGenerator::AnalyzeWords(TArray<FString> words, uint8 order)
{
	Order = order;

	Chains.Empty();
	WordPatterns.Empty();

	for(int i = 0; i < words.Num(); i++)
	{
		AnalyzeWord(words[i].ToLower());
	}

	CalculateProbability();
}

FMarkovCharacter* NameTileGenerator::GetCharacterByProbability(FString key, float probability)
{
	TArray<FMarkovCharacter> chain;
	if (!Chains.Contains(key))
	{
		UE_LOG(LogWorldGen, Warning, TEXT("Chain does not contain %s!"), *key);
		return NULL;
	}
	else
	{
		chain = Chains[key];
	}

	float cumulative = 0;
	FMarkovCharacter* result = NULL;

	for (int i = 0; i < chain.Num(); i++)
	{
		cumulative += chain[i].Probability;
		if (probability < cumulative)
		{
			result = new FMarkovCharacter();
			result->Instances = chain[i].Instances;
			result->Letter = chain[i].Letter;
			result->Probability = chain[i].Probability;
			break;
		}
	}

	return result;
}

FString NameTileGenerator::GenerateRandomWord(TArray<FText> Words, FRandomStream RandomGenerator, uint8 WordOrder, uint8 MinLength, uint8 MaxLength, bool bConvertToTitleCase, bool bMatchWordPattern)
{
	TArray<FString> wordStrings;
	wordStrings.SetNumZeroed(Words.Num());
	for(int i = 0; i < Words.Num(); i++)
	{
		wordStrings[i] = Words[i].ToString();
	}
	for(int i = 0; i < wordStrings.Num(); i++)
	{
		UE_LOG(LogWorldGen, Log, TEXT("%d: %s"), i, *wordStrings[i]);
	}
	AnalyzeWords(wordStrings, WordOrder);
	for (int i = 0; i < 64; i++)
	{
		FString word = GenerateRandomWord(MinLength, MaxLength, RandomGenerator, bConvertToTitleCase);
		UE_LOG(LogWorldGen, Log, TEXT("Generated word: %s"), *word);
		if (word.Len() < MinLength) continue;
		if (!bMatchWordPattern) return word;
		if (WordPatterns.Contains(GetWordPattern(word))) return word;
	}

	UE_LOG(LogWorldGen, Error, TEXT("Could not generate word!"));
	return FString();
}

FString NameTileGenerator::GenerateRandomWord(uint8 minLength, uint8 maxLength, FRandomStream& RandomGenerator, bool bConvertToTitleCase)
{
	std::string result;

	result = "";
	std::string key = "";
	for (int i = 0; i < Order; i++)
	{
		key += '_';
	}

	while (result.length() < maxLength)
	{
		FMarkovCharacter* character = GetCharacterByProbability(FString(key.c_str()), RandomGenerator.GetFraction());
		char ch = character == NULL ? '_' : character->Letter;
		delete character;
		if (ch == '_') break;

		result += ch;
		key += ch;

		key = key.substr(key.length() - Order);
	}
	if (bConvertToTitleCase)
	{
		return UppercaseWords(FString(result.c_str()));
	}
	else
	{
		result[0] = TChar<TCHAR>::ToUpper(result[0]);
		return FString(result.c_str());
	}
}

FString NameTileGenerator::UppercaseWords(FString value)
{
	TArray<TCHAR> charArray = value.GetCharArray();
	if (charArray.Num() >= 1)
	{
		if (TChar<TCHAR>::IsLower(charArray[0]))
		{
			charArray[0] = TChar<TCHAR>::ToUpper(charArray[0]);
		}
	}
	for (int i = 1; i < charArray.Num(); i++)
	{
		if (charArray[i - 1] == ' ')
		{
			if (TChar<TCHAR>::IsLower(charArray[i]))
			{
				charArray[i] = TChar<TCHAR>::ToUpper(charArray[i]);
			}
		}
	}
	std::string finishedString = "";
	for (int i = 0; i < charArray.Num(); i++)
	{
		finishedString += charArray[i];
	}
	return FString(finishedString.c_str());
}
