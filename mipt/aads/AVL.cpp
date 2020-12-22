#include <algorithm>
#include <cassert>
#include <memory>
#include <stack>


template <class T>
struct Node {
  Node() = default;
  explicit Node(const T& key_init)
    : key(key_init),
      height(1),
      size(1),
      left_subtree(nullptr),
      right_subtree(nullptr) {}

  T key;
  unsigned int height;
  size_t size;
  std::shared_ptr<Node<T>> left_subtree;
  std::shared_ptr<Node<T>> right_subtree;
};

template <class T, class Compare = std::less<T>>
class AVL {
 public:
  AVL() : root_(nullptr){};

  size_t GetSize() const { return GetSize_(root_); }

  void Clear() {
    root_ = nullptr;
  }

  void Insert(const T& value) {
    if (root_ == nullptr) {
      root_ = std::make_shared<Node<T>>(value);
      return;
    }

    std::shared_ptr<Node<T>> current_node = root_;
    std::stack<std::pair<std::shared_ptr<Node<T>>, ChildType_>> calls;
    while (true) {
      if (compare_(current_node->key, value)) {
        if (current_node->right_subtree != nullptr) {
          calls.push({current_node, ChildType_::Right});
          current_node = current_node->right_subtree;
        } else {
          current_node->right_subtree = std::make_shared<Node<T>>(value);
          break;
        }
      } else {
        if (current_node->left_subtree != nullptr) {
          calls.push({current_node, ChildType_::Left});
          current_node = current_node->left_subtree;
        } else {
          current_node->left_subtree = std::make_shared<Node<T>>(value);
          break;
        }
      }
    }

    Correct_(calls);
  }

  size_t FindIndexOfElement(const T& value) {
    size_t index = 0;
    std::shared_ptr<Node<T>> current_node = root_;
    while (true) {
      if (current_node == nullptr) {
        return index;
      }
      if (compare_(current_node->key, value)) {
        index += (GetSize_(current_node->left_subtree) + 1);
        current_node = current_node->right_subtree;
      } else if (compare_(value, current_node->key)) {
        current_node = current_node->left_subtree;
      } else {
        index += (GetSize_(current_node->left_subtree) + 1);
        break;
      }
    }
    return index;
  }

  T FindKthElement(size_t index) {
    --index;
    assert(index < GetSize_(root_));

    std::shared_ptr<Node<T>> current_node = root_;
    while (true) {
      if (GetSize_(current_node->left_subtree) < index) {
        index -= (GetSize_(current_node->left_subtree) + 1);
        current_node = current_node->right_subtree;
      } else if (GetSize_(current_node->left_subtree) > index) {
        current_node = current_node->left_subtree;
      } else {
        return current_node->key;
      }
    }
  }

  void Remove(const T& value) {
    std::shared_ptr<Node<T>> current_node = root_;
    if (!compare_(root_->key, value) && !compare_(value, root_->key)) {
      if (root_->left_subtree == nullptr) {
        root_ = root_->right_subtree;
        return;
      } else if (root_->right_subtree == nullptr) {
        root_ = root_->left_subtree;
        return;
      } else {
        std::shared_ptr<Node<T>> least = GetMin_(current_node->right_subtree);
        T changing_key = least->key;
        Remove(changing_key);
        current_node->key = changing_key;
        return;
      }
    }

    RemoveUpdate_(current_node, value);
  }

 private:
  enum class ChildType_ { Left, Right };

  void RemoveUpdate_(std::shared_ptr<Node<T>> current_node, const T& value) {
    std::stack<std::pair<std::shared_ptr<Node<T>>, ChildType_>> calls;
    while (true) {
      if (current_node == nullptr) {
        return;
      }
      if (compare_(current_node->key, value)) {
        calls.push({current_node, ChildType_::Right});
        current_node = current_node->right_subtree;
      } else if (compare_(value, current_node->key)) {
        calls.push({current_node, ChildType_::Left});
        current_node = current_node->left_subtree;
      } else {
        if (current_node->left_subtree == nullptr) {
          auto[parent, child_type] = calls.top();
          if (child_type == ChildType_::Left) {
            parent->left_subtree = current_node->right_subtree;
          } else {
            parent->right_subtree = current_node->right_subtree;
          }
        } else if (current_node->right_subtree == nullptr) {
          auto[parent, child_type] = calls.top();
          if (child_type == ChildType_::Left) {
            parent->left_subtree = current_node->left_subtree;
          } else {
            parent->right_subtree = current_node->left_subtree;
          }
        } else {
          std::shared_ptr<Node<T>> least = GetMin_(current_node->right_subtree);
          T changing_key = least->key;
          Remove(changing_key);
          current_node->key = changing_key;
          return;
        }
        break;
      }
    }

    Correct_(calls);
  }

  std::shared_ptr<Node<T>> GetMin_(std::shared_ptr<Node<T>> current_node) {
    if (current_node == nullptr) return nullptr;
    while (current_node->left_subtree != nullptr) {
      current_node = current_node->left_subtree;
    }
    return current_node;
  }

  unsigned int GetHeight_(std::shared_ptr<Node<T>> node) {
    if (node == nullptr) {
      return 0;
    } else {
      return node->height;
    }
  }

  size_t GetSize_(std::shared_ptr<Node<T>> node) const {
    if (node == nullptr) {
      return 0;
    } else {
      return node->size;
    }
  }

  int GetBalance_(std::shared_ptr<Node<T>> node) {
    if (node == nullptr) {
      return 0;
    } else {
      return GetHeight_(node->left_subtree) - GetHeight_(node->right_subtree);
    }
  }

  void UpdateHeight_(std::shared_ptr<Node<T>> node) {
    if (node == nullptr) return;
    node->height = std::max(GetHeight_(node->left_subtree),
                            GetHeight_(node->right_subtree)) +
                   1;
  }

  void UpdateSize_(std::shared_ptr<Node<T>> node) {
    if (node == nullptr) return;
    node->size =
            GetSize_(node->left_subtree) + GetSize_(node->right_subtree) + 1;
  }

  std::shared_ptr<Node<T>> SmallRightRotation_(
          std::shared_ptr<Node<T>> lowering_node) {
    std::shared_ptr<Node<T>> lifting_node = lowering_node->left_subtree;

    lowering_node->left_subtree = lifting_node->right_subtree;
    lifting_node->right_subtree = lowering_node;

    Update_(lowering_node);
    Update_(lifting_node);
    return lifting_node;
  }

  std::shared_ptr<Node<T>> SmallLeftRotation_(
          std::shared_ptr<Node<T>> lowering_node) {
    std::shared_ptr<Node<T>> lifting_node = lowering_node->right_subtree;

    lowering_node->right_subtree = lifting_node->left_subtree;
    lifting_node->left_subtree = lowering_node;

    Update_(lowering_node);
    Update_(lifting_node);
    return lifting_node;
  }

  std::shared_ptr<Node<T>> BigLeftRotation_(std::shared_ptr<Node<T>> node) {
    node->right_subtree = SmallRightRotation_(node->right_subtree);
    return SmallLeftRotation_(node);
  }

  std::shared_ptr<Node<T>> BigRightRotation_(std::shared_ptr<Node<T>> node) {
    node->left_subtree = SmallLeftRotation_(node->left_subtree);
    return SmallRightRotation_(node);
  }

  std::shared_ptr<Node<T>> CheckForRotation_(
          std::shared_ptr<Node<T>> current_node) {
    if (current_node == nullptr) {
      return nullptr;
    }

    std::shared_ptr<Node<T>> root_node = current_node;
    Update_(current_node->right_subtree);
    Update_(current_node->left_subtree);
    Update_(root_node);
    if (GetBalance_(current_node) > 1) {
      if (GetBalance_(current_node->left_subtree) > 0)
        root_node = SmallRightRotation_(current_node);
      else
        root_node = BigRightRotation_(current_node);
    } else if (GetBalance_(current_node) < -1) {
      if (GetBalance_(current_node->right_subtree) < 0)
        root_node = SmallLeftRotation_(current_node);
      else
        root_node = BigLeftRotation_(current_node);
    }

    return root_node;
  }

  void Update_(std::shared_ptr<Node<T>> node) {
    UpdateHeight_(node);
    UpdateSize_(node);
  }

  void Correct_(
          std::stack<std::pair<std::shared_ptr<Node<T>>, ChildType_>>& calls) {
    while (!calls.empty()) {
      auto[parent, child_type] = calls.top();
      calls.pop();
      if (child_type == ChildType_::Left) {
        Update_(parent->left_subtree);
        parent->left_subtree = CheckForRotation_(parent->left_subtree);
      } else {
        Update_(parent->right_subtree);
        parent->right_subtree = CheckForRotation_(parent->right_subtree);
      }
      Update_(parent);
    }
    root_ = CheckForRotation_(root_);
  }

  std::shared_ptr<Node<T>> root_;
  Compare compare_;
};
