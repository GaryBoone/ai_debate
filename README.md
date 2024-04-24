# The Great AI Debate

### GPT, Claude, and Gemini debate the issues of the day.

In this repo, three commercial AI chat systems debate each other. Who is the most convincing? Who produces the clearest arguments? Can they agree?

The code, in C++17, provides a unified interface for working with the GPT, Claude, and Gemini APIs. 

### Example output

> The debate proposition is 'AI will have a positive effect on knowledge workers'.
> 
> ======= Round 1 ==========
> 
> Claude: I believe AI will have an overall positive effect on knowledge workers. While there are understandable concerns about job displacement, I think the benefits outweigh the risks. AI has the potential to augment and enhance knowledge work in powerful ways:
> 
> 1) AI can automate routine and repetitive aspects of knowledge work, freeing up humans to focus on higher-level tasks that require creativity, strategic thinking, and human judgment. This could make knowledge work more engaging and fulfilling.  
> 
> 2) AI tools can help knowledge workers be more productive and efficient by assisting with research, data analysis, writing, and other core tasks. Working in tandem with AI, knowledge workers may be able to accomplish more.
> 
> 3) AI could help create entirely new categories of knowledge work and high-skill jobs. Just as past technological revolutions eliminated some jobs but created many others, AI may spur demand for new roles.
> 
> However, realizing these benefits will require proactively addressing the risks and challenges. We'll need robust worker retraining and upskilling programs, as well as strong social safety nets. Companies deploying AI should do so thoughtfully and ethically. Overall though, with the right approach, I believe AI will be a net positive for knowledge workers.
> 
> GPT: Claude presents an optimistic view about the role of AI in the future of knowledge work, and I largely concur with the points raised. Let's delve deeper into the arguments:
> 
> ...
> 
> Gemini: I find myself largely in agreement with both Claude and GPT regarding the potential positive impact of AI on knowledge workers.  However, I believe it's crucial to delve deeper into the potential challenges and complexities that accompany this optimistic outlook.
> 
> ...
> 
> Claude: Gemini, you raise excellent points about the potential challenges and complexities that may arise as AI becomes more integrated into knowledge work. I agree that we need to approach this transition with nuance and foresight.
> 
> ...



### Requirements

API keys are required for each Chatbot. The keys are read from environment variables: `GPT_API_KEY`, `CLAUDE_API_KEY`, and `GEMINI_API_KEY`. 

Tested on `macOS Sonoma`, 14.4.1.

### Code

The code mainly focuses on the interface for working with the GPT, Claude, and Gemini APIs. 

Chat clients are easily instantiated like:

```C
auto claude_client = APIClientFactory::CreateClaudeClient(anthropic_key);
```

The design combines common code in an client class and specializes via templates. That is,
the `ApiClient` class is specialized to each API via templating that provides the request formatting and responses processed unique to the API.

```C
auto gpt_client = ApiClient<GPTChunkProcessor, GPTRequestMaker>(api_key);
```

These can be extended to other APIs by writing additional classes that conform to the `IChunkProcessor` and `IRequestMaker` interfaces.

### Build

From the project directory, build using `CMake` then run the tests and main like this: 

```bash
$ cmake -S . -B build
$ cmake --build build
$ ./build/ai_debate_tests
$ ./build/ai_debate_app
```