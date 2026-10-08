#include <memory>
#include <vector>
#include <unordered_map>
#include "GameObject.h"
#include "NotificationManager.h"

namespace CMPUT350 {
    void NotificationManager::Register(std::weak_ptr<GameObject> object, const std::string& key) {

    }

    void NotificationManager::Unregister(std::weak_ptr<GameObject> object, const std::string& key) {

    }

    void NotificationManager::Notify(const std::string& message) {

    }
}