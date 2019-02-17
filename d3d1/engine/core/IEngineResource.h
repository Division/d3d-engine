#pragma once

class IEngineResource {
public:
	virtual bool isLoading() const = 0;
	virtual bool isReady() const = 0;

	virtual ~IEngineResource() = default;
};
