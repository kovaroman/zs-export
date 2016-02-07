<?hh // strict
namespace ElasticExport\Generators;


use ElasticExport\Helper\ItemDescriptionHelper;
use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\DataExchange\Models\FormatSetting;

/**
 * Class IdealoGenerator
 * @package ElasticExport\Generators
 */
class IdealoGenerator extends CSVGenerator
{
    /*
     * @var
     */
    private ItemDescriptionHelper $itemDescriptionHelper;

    /**
     * IdealoGenerator constructor.
     * @param ItemDescriptionHelper $itemDescriptionHelper
     */
    public function __construct(ItemDescriptionHelper $itemDescriptionHelper)
    {
        $this->itemDescriptionHelper = $itemDescriptionHelper;
    }

    protected function generateContent(mixed $resultData, array<FormatSetting> $formatSettings = []):void
    {
        //code here
    }
}