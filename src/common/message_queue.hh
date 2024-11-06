#ifndef SIMULATIONS_COMMON_MESSAGE_QUEUE
#define SIMULATIONS_COMMON_MESSAGE_QUEUE

#include "pch.hh"

template <typename T>
class MessageQueueReader;

template <typename T>
class MessageQueueWriter;

template <typename T>
class MessageQueue {
public:
  ~MessageQueue() = default;

  static std::pair<std::unique_ptr<MessageQueueReader<T>>, std::shared_ptr<MessageQueueWriter<T>>> create() {
    auto message_queue = std::shared_ptr<MessageQueue<T>>(new MessageQueue<T>);
    auto reader = std::unique_ptr<MessageQueueReader<T>>(new MessageQueueReader<T>(message_queue));
    auto writer = std::shared_ptr<MessageQueueWriter<T>>(new MessageQueueWriter<T>(message_queue));
    return {std::move(reader), std::move(writer)};
  }

  void push(T&& msg) {
    std::lock_guard<std::mutex> lock(m_queue_mtx);
    m_messages.push(msg);
  }

  T pop() {
    std::lock_guard<std::mutex> lock(m_queue_mtx);
    auto msg = m_messages.front();
    m_messages.pop();
    return msg;
  }

  void foreach(std::function<void(T)> func) {
    std::lock_guard<std::mutex> lock(m_queue_mtx);
    while (!m_messages.empty()) {
      auto msg = m_messages.front();
      m_messages.pop();
      func(std::move(msg));
    }
  }

private:
  std::queue<T> m_messages;
  std::mutex m_queue_mtx;

  MessageQueue() : m_messages{}, m_queue_mtx{} {}
};

template <typename T>
class MessageQueueReader {
  public:
  ~MessageQueueReader() = default;

  T pop() {
    return m_queue->pop();
  }

  void foreach(std::function<void(T)> func) {
    m_queue->foreach(func);
  }

  private:
    std::shared_ptr<MessageQueue<T>> m_queue;
    
    MessageQueueReader(std::shared_ptr<MessageQueue<T>> message_queue) : m_queue{message_queue} {}

  friend MessageQueue<T>;
};

class MessageQueueExpired : public std::exception {
  public:
    virtual char const* what() const noexcept override {
      return "Accessed message queue has expired";
    }
};

template <typename T>
class MessageQueueWriter {
  public:
    ~MessageQueueWriter() = default;

    void push(T&& msg) {
      if (auto queue = m_queue.lock())
        queue->push(std::move(msg));
      else
        throw MessageQueueExpired();
    }

  private:
    std::weak_ptr<MessageQueue<T>> m_queue;
    MessageQueueWriter(std::shared_ptr<MessageQueue<T>> message_queue) : m_queue{message_queue} {}

  friend MessageQueue<T>;
};

#endif // SIMULATIONS_COMMON_MESSAGE_QUEUE