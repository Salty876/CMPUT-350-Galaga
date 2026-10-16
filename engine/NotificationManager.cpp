#include <memory>
#include <vector>
#include <unordered_map>
#include "GameObject.h"
#include "NotificationManager.h"

namespace CMPUT350 {
    void NotificationManager::Register(std::weak_ptr<GameObject> object, const std::string& key) {
        // key gets added for the first time
        if (!mListeners.contains(key)) {
            mListeners.insert({key, {object}});
        }
        // we already have the key in the map
        else {
            mListeners.at(key).push_back(object);
        }
    }

    void NotificationManager::Unregister(std::weak_ptr<GameObject> object, const std::string& key) {
        std::vector<std::weak_ptr<GameObject>> targetObjects = mListeners[key];
        std::vector<std::weak_ptr<GameObject>> newTargetObjects; 
        //make new vector with objects I wanna keep
        for (auto obj: targetObjects) {
            if (obj.lock() != nullptr) {
                if (obj.lock() != object.lock()) {
                    newTargetObjects.push_back(obj);
                }
            }

        }

        mListeners.insert_or_assign(key, newTargetObjects);
    }

    void NotificationManager::Notify(const std::string& message) {
        std::vector<std::weak_ptr<GameObject>> targets = mListeners[message];
        for (auto obj: targets) {
            if (obj.lock() != nullptr) {
                obj.lock()->ReceiveNotification(message);
            }
        }
    }
}