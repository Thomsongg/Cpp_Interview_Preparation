#include <iostream>
#include <fstream>      // 读写文件
#include <string>
#include <vector>
#include <nlohmann/json.hpp>	// json 操作库

using namespace std;
using json = nlohmann::json;

/*
 * json 字符串的解析
 */

// json 文件的读写
void jsonFileExample(json data)
{
    // json 文件内容的读取
    //ifstream i("config.json");
    //json config;
    //i >> config;

    // 写入内容到 json 文件
    ofstream o("output.json");
    o << setw(4) << data << "\n";   // 设置 4 格缩进
}

void processAiResponse(const string& raw_json)
{
    try
    {
        // 1. 将 json 字符串解析为 json 对象（C++ 可处理）
        json data = json::parse(raw_json);

        // 2. 简单的字段访问
        // (1) value(字段值，默认值)
        // (2) 使用索引
        string model_name = data.value("model", "unknown");
        int version = data["metadata"]["version"];
        string device_name = data["metadata"]["device"];

        cout << "Model name: " << model_name << "\n";
        cout << "Metadata:\n";
        cout << "\tVersion: V" << version << ", Device: " << device_name << "\n";

        // 3. 处理某个字段的数组
        // 需提前确保字段存在，且为数组
        string param_name = "predictions";
        if (data.contains(param_name) && data[param_name].is_array())
        {
            auto predictions = data[param_name];

            for (const auto& item : predictions)
            {
                string label_name = item["label"];
                double score = item["score"];

                if (score > 0.8)
                {
                    cout << "[High confidence] " << label_name << " get score of " << score << "\n";
                }
                else
                {
                    cout << "[Low  confidence] " << label_name << " get score of " << score << "\n";
                }
            }
        }

        // 4. json 字符串的修改（及添加）
        // data.dump(4) 表示缩进 4 格
        data["processed"] = false;
        data["metadata"]["GPU"] = "RTX 4060 Ti";

        // 在数组中添加一行新数据
        data["predictions"].push_back({
            {"label", "pony"},
            {"score", 0.05}
        });

        // 可单独创建 json 对象后，将对象整体加入
        json new_predictions = {
            {"label", "bull"},
            {"score", 0.006}
        };

        data["predictions"].push_back(new_predictions);
        cout << "\nModified Json: " << data.dump(4) << "\n";

        // 遍历并输出 key-value
        json metadata = { {"device", "cuda"}, {"batch_size", 8}, {"precision", "fp16"} };
        for (auto& [key, value] : metadata.items())
        {
            cout << "Key: " << key << ", Value: " << value << "\n";
        }

        //jsonFileExample(data);

        // 【重要】json 数据的清洗（删除）
        // 目标：删除所有 score < 0.1 的元素
        // 使用迭代器删除!
        auto& predictions = data["predictions"];
        for (auto it = predictions.begin(); it != predictions.end(); )
        {
            if ((*it)["score"] < 0.1)
            {
                // erase 删除，并返回下一个迭代器
                it = predictions.erase(it);
            }
            else
            {
                it++;
            }
        }
        data["processed"] = true;

        cout << "\nDeleted Json: " << data.dump(4) << "\n";
    }
    catch (json::parse_error& e)
    {
        cout << "Json Parse Qrror: " << e.what() << "!\n";
    }
}

// 示例：实现 结构体 - json 的自动映射
struct Detection
{
    string label;
    float score;
    vector<int> bbox;
};

// 使用宏，实现自动序列化与反序列化
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Detection, label, score, bbox)

void example()
{
    // 准备 C++ 数据
    std::vector<Detection> dets = {
        {"cat", 0.9f, {10, 10, 50, 50}},
        {"dog", 0.8f, {100, 200, 30, 40}}
    };

    // 反序列化: vector<struct> -> json 对象
    json data = dets;
    cout << data.dump(4) << "\n";

    // 序列化: json 对象 -> vector<struct>
    auto dets_back = data.get<vector<Detection>>();
}

int main()
{
    // 模拟一段从 API 获取的 JSON 字符串
    std::string json_input = R"(
    {
        "model": "resnet50",
        "metadata": {"version": 2, "device": "cuda"},
        "predictions": [
            {"label": "cat", "score": 0.95},
            {"label": "dog", "score": 0.12}
        ]
    }
    )";

    cout << "Basic info:\n";

    processAiResponse(json_input);

    //example();
    return 0;
}
