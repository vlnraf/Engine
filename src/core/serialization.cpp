#include "serialization.hpp"
#include "core/tracelog.hpp"

SerializationState initSerializer(const char* filePath){
    SerializationState state = {};
    state.filePath = filePath;
    state.bufPos = 0;
    state.indentLevel = 0;
    state.indentList = -1;

    FILE *outFile = fopen(state.filePath, "w");
    if (outFile) {
        fclose(outFile);
    } else {
        perror("Error opening file for writing");
        fclose(outFile);
    }
    return state;
}

bool checkBufferSpace(SerializationState* state, size_t requiredSize) {
    if (state->bufPos + requiredSize >= BUFFER_SIZE) {
        return true;
    }else{
        return false;
    }
}

void writeBufferIfPossible(SerializationState* state, size_t requiredSize){
    if(checkBufferSpace(state, requiredSize)){
        state->bufPos += requiredSize;
    }else{
        state->bufPos = 0;
        serializeWriteFile(state);
    }
}

void addIndentation(SerializationState* state){
    for (int i = 0; i < state->indentLevel; i++) {
        state->bufPos += std::sprintf(state->buffer + state->bufPos, "  "); // Two spaces per level
    }
}

void serializeObjectStart(SerializationState* state, const char* name){
    addIndentation(state);
    int written = std::sprintf(state->buffer + state->bufPos, "%s:\n", name);
    writeBufferIfPossible(state, written);
    state->indentLevel++;
}

void serializeObjectEnd(SerializationState* state){
    state->indentLevel--;
}

void serializeListStart(SerializationState* state, const char* name){
    addIndentation(state);
    int written = std::sprintf(state->buffer + state->bufPos, "%s:\n", name);
    writeBufferIfPossible(state, written);
    state->indentLevel++;
    state->indentList = state->indentLevel;
}

void serializeListEnd(SerializationState* state){
    state->indentLevel--;
    state->indentList = state->indentLevel;
}

void addList(SerializationState* state){
    if(state->indentList == state->indentLevel){
        int written = std::sprintf(state->buffer + state->bufPos, "- ");
        writeBufferIfPossible(state, written);
    }
}

void serializeItemsStart(SerializationState* state){
    state->indentLevel++;
}

void serializeItemsEnd(SerializationState* state){
    state->indentLevel--;
}

void serializeVec2(SerializationState* state, const char* name, const glm::vec2* v){
    addIndentation(state);
    addList(state);
    int written = std::sprintf(state->buffer + state->bufPos, "%s: [%.3f, %.3f]\n", name, v->x, v->y);
    writeBufferIfPossible(state, written);
}

void serializeVec3(SerializationState* state, const char* name, const glm::vec3* v){
    addIndentation(state);
    addList(state);
    int written = std::sprintf(state->buffer + state->bufPos, "%s: [%.3f, %.3f, %.3f]\n", name, v->x, v->y, v->z);
    state->bufPos += written;
    writeBufferIfPossible(state, written);
}

void serializeInt(SerializationState* state, const char* name, const int v){
    addIndentation(state);
    addList(state);
    int written = std::sprintf(state->buffer + state->bufPos, "%s: %d\n", name, v);
    state->bufPos += written;
    writeBufferIfPossible(state, written);
}

void serializeBool(SerializationState* state, const char* name, const bool v){
    addIndentation(state);
    addList(state);
    int written;
    if(v){
        written = std::sprintf(state->buffer + state->bufPos, "%s: %s\n", name, "True");
    }else{
        written = std::sprintf(state->buffer + state->bufPos, "%s: %s\n", name, "False");
    }
    state->bufPos += written;
    writeBufferIfPossible(state, written);
}

void serializeFloat(SerializationState* state, const char* name, const float v){
    addIndentation(state);
    addList(state);
    int written = std::sprintf(state->buffer + state->bufPos, "%s: %.3f\n", name, v);
    state->bufPos += written;
    writeBufferIfPossible(state, written);
}

void serializeString(SerializationState* state, const char* name, const char* v){
    addIndentation(state);
    addList(state);
    int written = std::sprintf(state->buffer + state->bufPos, "%s: %s\n", name, v);
    state->bufPos += written;
    writeBufferIfPossible(state, written);
}

void serializeKeyValue(SerializationState* state, const char* key, const char* value) {
    addIndentation(state);
    addList(state);
    state->bufPos += std::sprintf(state->buffer + state->bufPos, "%s: %s\n", key, value);
    writeBufferIfPossible(state, state->bufPos); 
}

void serializeWriteFile(SerializationState* state){
    FILE *outFile = fopen(state->filePath, "a");
    if (outFile) {
        fputs(state->buffer, outFile);
        fclose(outFile);
        state->bufPos = 0;
    } else {
        fclose(outFile);
        LOGERROR("Error opening file for writing");
    }
}


//-------------------------------Parser-Deserializer---------------------------------------
Node parseToNode(FILE* file);

enum TokenType{
    KEY,
    VALUE,
    INDENT,
    DEDENT,
    DASH
};

struct Token{
    TokenType type;
    std::string value;
    uint32_t indent;
};

Node serializeReadFile(const char* filePath){
    FILE *inFile = fopen(filePath, "r");
    
    if (inFile) {
        Node result = parseToNode(inFile);
        if(result.childrens.size() <= 0){
            LOGERROR("Error in parsing the file!");
            return {};
        }
        fclose(inFile);
        return result;
    } else {
        LOGERROR("Error opening file for reading");
        fclose(inFile);
        return {};
    }
    fclose(inFile);
}

bool readLine(std::string* line, FILE* file){
    char c;
    line->clear();

    while((c = fgetc(file)) != '\n' && c != EOF){
        line->push_back(c);
    }

    if (line->empty()) {
        return false;
    }

    return true;
}

void tokenizeLine(std::string line, std::vector<Token>* tokens, uint32_t* previousIndent){

    std::string key;
    std::string value;

    uint32_t indent = 0;

    while (indent < line.size() && line[indent] == ' ') {
            indent++;
    }

    if (indent > *previousIndent) {
        tokens->push_back({TokenType::INDENT, "INDENT", indent});
    } else if (indent < *previousIndent) {
        uint32_t deindent = *previousIndent - indent;
        for(uint32_t i = 0; i < deindent / 2; i++){
            tokens->push_back({TokenType::DEDENT, "DEDENT", deindent});
        }
    }
    *previousIndent = indent;

    line = line.substr(indent);

    if (line[0] == '-') {
        tokens->push_back({TokenType::DASH, "-", indent});
        line = line.substr(1); // Remove dash
    }

    size_t colPos = line.find(':');

    for(size_t pos = 0; pos < line.size(); pos++){
        if(pos < colPos){
            key.push_back(line[pos]);
        }else if( pos > colPos){
            if(line[pos] == ' '){
                continue;
            }
            value.push_back(line[pos]);
        }
    }
    Token tokenKey{};
    tokenKey.type = TokenType::KEY;
    tokenKey.value = key;
    tokens->push_back(tokenKey);
    Token tokenValue{};
    tokenValue.type = TokenType::VALUE;
    tokenValue.value = value;
    tokens->push_back(tokenValue);
}

//deprecated
void printAST(Node* node, int level){
    for (int i = 0; i < level; i++) {
        printf("  ");
    }

    // Print the key and value
    if (node->isList) {
        printf("- ");
    }else{
        if (!node->key.empty()) {
            printf("%s", node->key.c_str());
        }

        if (!node->value.empty()) {
            printf("%s", node->value.c_str());
        }
        printf("\n");
    }


    for (Node child : node->childrens) {
        printAST(&child, level + 1);
    }
}

Node tokenToAST(std::vector<Token> tokens){
    Node root;
    std::vector<Node*> stack;

    size_t i = 0;
    if(!(tokens[i].type == TokenType::KEY)){
        LOGERROR("The file should have a root node!");
        return {};
    }
    root.key = tokens[i].value;
    i++;
    if(tokens[i].type == TokenType::VALUE){
        root.value = tokens[i].value;
        i++;
    }

    stack.push_back(&root);

    //bool isChild = false;
    for(; i < tokens.size(); i++){
        Token t = tokens[i];
        if(t.type == TokenType::INDENT){
            stack.push_back(&stack.back()->childrens.back()); //decrease stack to the last node - 1
        }else if(t.type == TokenType::DEDENT){
            stack.pop_back(); //decrease stack to the last node - 1
        }else if(t.type == TokenType::KEY){
            Node n;
            n.key = t.value;
            if(i+1 < tokens.size() && tokens[i+1].type == TokenType::VALUE){
                n.value = tokens[++i].value; //Get value of the key and consume with ++i
            }
            stack.back()->childrens.push_back(n);
        }else if(t.type == TokenType::DASH){
            Node n;
            n.isList = true;

            if(i+1 < tokens.size() && tokens[i+1].type == TokenType::KEY){
                n.key = tokens[++i].value;
            }
            if(i+1 < tokens.size() && tokens[i+1].type == TokenType::VALUE){
                n.value = tokens[++i].value;
            }
            stack.back()->childrens.push_back(n);
        }
    }
    //printAST(&root, 0);

    return root;
}

Node* getNode(Node* node, const char* nodeName){
    for(Node& n : node->childrens){
        if(strcmp(n.key.c_str(), nodeName) == 0){
            return &n;
        }
    }
    //return std::vector<Node>();
    return nullptr;
}

Node parseToNode(FILE* file){
    std::string line;
    std::vector<Token> tokens;

    uint32_t previousIndent = 0;
    while(readLine(&line, file)){
        tokenizeLine(line, &tokens, &previousIndent);
    }

    Node root = tokenToAST(tokens);
    if(root.childrens.size() <= 0){
        return {};
    }
    return root;

    //std::vector<Node> entities = getNode(&root, "Entities");
    //if(entities.size() != 0){
    //    for(Node n : entities){
    //        std::vector<Node> components = getNode(n, "TransformComponent");
    //        for(Node c : components){
    //            printf("%s | %s\n", c.key.c_str(), c.value.c_str());
    //        }
    //    }
    //}


    //for(Node n : root.childrens){
    //    //printf("%s | %s\n", n.key.c_str(), n.value.c_str());
    //    for(Node a : n.childrens){
    //        //printf("%s | %s\n", a.key.c_str(), a.value.c_str());
    //        for(Node comp : a.childrens){
    //            if(comp.key == "TransformComponent"){
    //                for(Node t : comp.childrens){
    //                    if(t.key == "position"){
    //                        printf("%s\n",t.value.c_str());
    //                    }
    //                }
    //            }
    //        }
    ////        if(a.key == "- Entity"){
    ////        }
    //    }
    //}
}